#include "GlLifecycleUtils.hpp"

namespace gl_lifecycle {

bool isValidProgramBufferPair(GLuint program, GLuint buffer) {
  return program != 0 && buffer != 0 && glIsProgram(program) && glIsBuffer(buffer);
}

bool isValidProgramShaderSet(GLuint program, std::initializer_list<GLuint> shaders) {
  if (program == 0 || !glIsProgram(program)) {
    return false;
  }
  for (GLuint shader : shaders) {
    if (shader == 0 || !glIsShader(shader)) {
      return false;
    }
  }
  return true;
}

bool isValidTextureFramebufferPair(GLuint texture, GLuint framebuffer) {
  return texture != 0 && framebuffer != 0 && glIsTexture(texture) && glIsFramebuffer(framebuffer);
}

bool areValidTextures(std::initializer_list<GLuint> textures) {
  for (GLuint texture : textures) {
    if (texture == 0 || !glIsTexture(texture)) {
      return false;
    }
  }
  return true;
}

}  // namespace gl_lifecycle
