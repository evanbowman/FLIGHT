#pragma once

#ifdef FLIGHT_MAC
#include <OpenGL/gl3.h>
#else
#include <GL/glew.h>
#endif
#include <sstream>

void AssertGLStatus(const std::string & context);
