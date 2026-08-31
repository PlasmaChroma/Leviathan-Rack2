#pragma once

#include "plugin.hpp"
#include <initializer_list>

namespace gl_lifecycle {

bool isValidProgramBufferPair(GLuint program, GLuint buffer);
bool isValidProgramShaderSet(GLuint program, std::initializer_list<GLuint> shaders);
bool isValidTextureFramebufferPair(GLuint texture, GLuint framebuffer);
bool areValidTextures(std::initializer_list<GLuint> textures);

}  // namespace gl_lifecycle
