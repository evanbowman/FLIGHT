// #include "Font.hpp"

// static FT_Library ft;

// FontFace::FontFace() : m_characters{}, m_atlasWidth(0), m_atlasHeight(0) {}

// void FontFace::Init() {
//     if (FT_Init_FreeType(&ft)) {
// 	throw std::runtime_error("failed to initialize freetype");
//     }
// }

// void FontFace::CreateFromFile(const std::string & srcPath, const int size) {
//     FT_Face face;
//     if (FT_New_Face(ft, srcPath.c_str(), 0, &face)) {
// 	throw std::runtime_error("failed to open font from " + srcPath);
//     }
//     FT_Set_Pixel_Sizes(face, 0, size);
//     glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
//     for (GLubyte c = 0; c < 128; c++) {
// 	if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
// 	    continue;
// 	}
// 	GLuint texture;
// 	glGenTextures(1, &texture);
// 	glBindTexture(GL_TEXTURE_2D, texture);
// 	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, face->glyph->bitmap.width,
// 		     face->glyph->bitmap.rows, 0, GL_RED, GL_UNSIGNED_BYTE,
// 		     face->glyph->bitmap.buffer);
// 	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
// 	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
// 	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
// 	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
// 	m_characters[(int)c] = {
// 	    texture,
// 	    glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
// 	    glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
// 	    face->glyph->advance.x
// 	};
//     }
//     FT_Done_Face(face);
//     AssertGLStatus("creation of " + srcPath);
// }

// const Character & FontFace::GetChar(const char c) const {
//     return m_characters[(int)c];
// }

// void Text::SetFontFace(std::shared_ptr<FontFace> face) {
//     m_face = face;
// }

// void Text::SetString(const std::string & msg) {
//     m_msg = msg;
// }

// GLuint Text::m_vbo;

// void Text::Init() {
//     glGenBuffers(1, &m_vbo);
//     glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
//     glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, nullptr, GL_DYNAMIC_DRAW);
//     glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
// }

// void Text::Display(const GLuint shaderProg, const BlendMode mode) {
//     if (auto fontFaceSp = m_face.lock()) {
// 	float x = 0, y = 0;
// 	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
// 	glUniform1i(glGetUniformLocation(shaderProg, "tex"), 0);
// 	glActiveTexture(GL_TEXTURE0);
// 	const GLint vertLoc = glGetAttribLocation(shaderProg, "vertex");
// 	glBlendFunc(static_cast<GLenum>(mode.src), static_cast<GLenum>(mode.dest));
// 	for (const char c : m_msg) {
// 	    const Character & ch = fontFaceSp->GetChar(c);
// 	    glBindTexture(GL_TEXTURE_2D, ch.textureId);
// 	    const float xpos = x + ch.bearing.x;
// 	    const float ypos = y - (ch.size.y - ch.bearing.y);
// 	    GLfloat w = ch.size.x;
// 	    GLfloat h = ch.size.y;
// 	    float vertices[6][4] = {
// 		{ xpos,     ypos + h,   0.0, 0.0 },            
// 		{ xpos,     ypos,       0.0, 1.0 },
// 		{ xpos + w, ypos,       1.0, 1.0 },
		
// 		{ xpos,     ypos + h,   0.0, 0.0 },
// 		{ xpos + w, ypos,       1.0, 1.0 },
// 		{ xpos + w, ypos + h,   1.0, 0.0 }           
// 	    };
// 	    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
// 	    glVertexAttribPointer(vertLoc, 4, GL_FLOAT, GL_FALSE, 0, 0);
// 	    glDrawArrays(GL_TRIANGLES, 0, 6);
// 	    x += (ch.advance >> 6); 
// 	}
// 	glBlendFunc(GL_ONE, GL_ZERO);
// 	glBindBuffer(GL_ARRAY_BUFFER, 0);
// 	glBindTexture(GL_TEXTURE_2D, 0);
//     }
//     AssertGLStatus("text rendering");
// }
