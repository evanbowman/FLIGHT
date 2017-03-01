// #pragma once

// #include <stdexcept>
// #include <vector>
// #include <memory>
// #include <string>
// #include <algorithm>
// #include <glm/glm.hpp>
// #include <OpenGL/gl3.h>
// #include "BlendMode.hpp"
// #include <ft2build.h>
// #include FT_FREETYPE_H
// #include "Error.hpp"

// struct Character {
//     GLuint textureId;
//     glm::ivec2 size;
//     glm::ivec2 bearing;
//     long advance;
// };

// class FontFace : public std::enable_shared_from_this<FontFace> {
//     friend class Game;
//     static void Init();
//     Character m_characters[128];
//     int m_atlasWidth, m_atlasHeight;
// public:
//     FontFace();
//     const Character & GetChar(const char c) const;
//     void CreateFromFile(const std::string & srcPath, const int size);
// };

// class Text {
//     friend class Game;
//     static void Init();
//     std::weak_ptr<FontFace> m_face;
//     std::string m_msg;
//     static GLuint m_vbo;
// public:
//     void SetFontFace(std::shared_ptr<FontFace> face);
//     void SetString(const std::string & msg);
//     void Display(const GLuint shaderProg, const BlendMode mode);
// };
