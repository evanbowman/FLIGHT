#include "Font.hpp"
#include "Game.hpp"

int CalcFontSize(FontSizeFactor size) {
    auto screenSize = GetGame().GetWindowSize();
    switch (size) {
    case FontSizeFactor::Large:
	return ((screenSize.x + screenSize.y) / 2) * 0.1f;
	break;

    case FontSizeFactor::Medium:
	return ((screenSize.x + screenSize.y) / 2) * 0.05f;
	break;

    case FontSizeFactor::Small:
	return ((screenSize.x + screenSize.y) / 2) * 0.025f;
	break;
    }
}

void FontAtlas::LoadFromFile(const std::string & path,
			     const std::initializer_list<int> & sizes) {
    m_texAtlas = texture_atlas_new(1024, 1024, 1);
    for (int size : sizes) {
	texture_font_t * texture;
	texture = texture_font_new_from_file(m_texAtlas, size, path.c_str());
	if (!texture) {
	    throw std::runtime_error("failed to create font from " + path);
	}
	m_fonts.push_back({texture, size});
    }
    glGenTextures(1, &m_texAtlas->id );
    glBindTexture(GL_TEXTURE_2D, m_texAtlas->id );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, m_texAtlas->width, m_texAtlas->height,
		 0, GL_RED, GL_UNSIGNED_BYTE, m_texAtlas->data );
    AssertGLStatus("font loading");
}

const Font & FontAtlas::GetFont(FontSizeFactor size) const {
    const int fsize = CalcFontSize(size);
    auto font = std::find_if(m_fonts.begin(), m_fonts.end(), [fsize](const Font & font) {
								 return font.size == fsize;
							     });
    if (font == m_fonts.end()) {
	throw std::runtime_error("font size does not exist");
    }
    return *font;
}

void Text::SetFontAtlas(std::shared_ptr<FontAtlas> atlas) {
    m_fontAtlas = atlas;
}

typedef struct {
    float x, y, z;
    float s, t; 
} vertex_t;

void Text::Create(const std::string & msg, FontSizeFactor size) {
    if (auto fontAtlasSp = m_fontAtlas.lock()) {
	auto & font = fontAtlasSp->GetFont(size);
	int x = 0;
	std::vector<vertex_t> vertices;
	for (size_t i = 0; i < msg.size(); ++i) {
	    texture_glyph_t * glyph = texture_font_get_glyph(font.texture, &msg[i]);
	    if (glyph != NULL) {
		float kerning =  0.0f;
		if (i > 0) {
		    kerning = texture_glyph_get_kerning( glyph, &msg[i - 1]);
		}
		x += kerning;
		int x0  = (int)( x + glyph->offset_x );
		int y0  = (int)( glyph->offset_y );
		int x1  = (int)( x0 + glyph->width );
		int y1  = (int)( y0 - glyph->height );
		float s0 = glyph->s0;
		float t0 = glyph->t0;
		float s1 = glyph->s1;
		float t1 = glyph->t1;
		
		vertices.push_back({(float)x0, (float)y0, 0, s0, t0});
		vertices.push_back({(float)x0, (float)y1, 0, s0, t1});
		vertices.push_back({(float)x1, (float)y1, 0, s1, t1});
		
		vertices.push_back({(float)x0, (float)y0, 0, s0, t0});
		vertices.push_back({(float)x1, (float)y1, 0, s1, t1});
		vertices.push_back({(float)x1, (float)y0, 0, s1, t0});
		x += glyph->advance_x;
	    }
	}
	glGenBuffers(1, &m_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
        m_bufSize = sizeof(vertex_t) * vertices.size();
	glBufferData(GL_ARRAY_BUFFER, m_bufSize, vertices.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	m_initialized = true;
    } else {
	throw std::runtime_error("could not create text \'" + msg + "\', missing font");
    }
    AssertGLStatus("text creation");
}

GLuint FontAtlas::GetTexture() const {
    return m_texAtlas->id;
}

void Text::Display(const GLuint shaderProg, const BlendMode & blendMode) {
    if (!m_initialized) {
	throw std::runtime_error("attempt to display uninitialized text (did you forget to call Text::Create()?)");
    }
    if (auto fontAtlasSp = m_fontAtlas.lock()) {
	glActiveTexture(GL_TEXTURE0);
	glUniform1i(glGetUniformLocation(shaderProg, "tex"), 0);
	glBindTexture(GL_TEXTURE_2D, fontAtlasSp->GetTexture());
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	GLint posAttrib = glGetAttribLocation(shaderProg, "position");
	GLint texAttrib = glGetAttribLocation(shaderProg, "texCoord");
	glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), 0);
	glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(3 * sizeof(float)));
	glBlendFunc(static_cast<GLenum>(blendMode.src),
		    static_cast<GLenum>(blendMode.dest));
	glDrawArrays(GL_TRIANGLES, 0, m_bufSize);
	glBlendFunc(GL_ONE, GL_ZERO);
	glBindTexture(GL_TEXTURE_2D, 0);
    }
    AssertGLStatus("font rendering");
}

Text::Text() : m_initialized(false) {}

Text::Text(Text && other) {
    m_initialized = other.m_initialized;
    other.m_initialized = false;
    m_vbo = other.m_vbo;
    m_fontAtlas = other.m_fontAtlas;
}

const Text & Text::operator=(Text && other) {
    m_initialized = other.m_initialized;
    m_vbo = other.m_vbo;
    m_fontAtlas = other.m_fontAtlas;
    other.m_initialized = false;
    return *this;
}

Text::~Text() {
    if (m_initialized) {
	// cleanup vertex buffer...
    }
}
