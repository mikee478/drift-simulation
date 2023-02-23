#include "renderer.h"
#include <GL/glew.h>

void GLClearErrors()
{
    while(glGetError() != GL_NO_ERROR);
}

bool GLLogCall(const char* function, const char* file, int line)
{
    GLenum error;
    while((error = glGetError()) != GL_NO_ERROR)
    {
        std::cout << "OpenGL Error - (" << error << ") - " << function << " " << file << ":" << line << std::endl;
        return false;
    }
    return true;
}

Renderer::Renderer()
{
    glEnable(GL_DEPTH_TEST);
}

void Renderer::Clear() const
{
    // Clears color buffer
    GLCALL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
}

void Renderer::Draw(unsigned int mode, const VertexArray& va, const Shader& shader) const
{
    va.Bind();
    shader.Bind();

    int size = 0;
    GLCALL(glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size));
    
    int count = 0;
    switch(mode)
    {
        case GL_TRIANGLES:
            count = size / sizeof(unsigned int);
            break;
        case GL_POINTS:
            count = size / sizeof(unsigned int);
            break;
        default:
            std::cout << "Error - draw mode not implemented" << std::endl;
            std::cin.get();
            count = 0;
    }

    /* glDrawElements
         * mode - kind of primitives to render
         * count - uses count sequential elements from an enabled array
         * type - type of the values in indices
         * indices - offset of the first index to be used from GL_ELEMENT_ARRAY_BUFFER target buffer
    */
    GLCALL(glDrawElements(mode, count, GL_UNSIGNED_INT, nullptr));
}