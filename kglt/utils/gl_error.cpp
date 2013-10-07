#include <GL/glew.h>
#include <boost/format.hpp>

#include "../kazbase/logging.h"
#include "../kazbase/exceptions.h"

void check_and_log_error(std::string file, int lineno) {
    GLuint error = glGetError();
    if(error != GL_NO_ERROR) {
        unicode error_string;
        switch(error) {
        case GL_INVALID_ENUM:
            error_string = "GL_INVALID_ENUM";
            break;
        case GL_INVALID_VALUE:
            error_string = "GL_INVALID_VALUE";
            break;
        case GL_INVALID_OPERATION:
            error_string = "GL_INVALID_OPERATION";
            break;
        case GL_INVALID_FRAMEBUFFER_OPERATION:
            error_string = "GL_INVALID_FRAMEBUFFER_OPERATION";
            break;
        case GL_OUT_OF_MEMORY:
            error_string = "GL_OUT_OF_MEMORY";
            break;
        case GL_STACK_OVERFLOW:
            error_string = "GL_STACK_OVERFLOW";
            break;
        case GL_STACK_UNDERFLOW:
            error_string = "GL_STACK_UNDERFLOW";
            break;
        }

        L_ERROR((boost::format("An OpenGL error occurred: %s:%d - %d") % file % lineno % error).str());
        throw RuntimeError(_u("GL ERROR: {0}").format(error_string).encode());
    }
}

