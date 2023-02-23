#include "vertex_buffer.h"
#include "renderer.h"

#include <GL/glew.h>

VertexBuffer::VertexBuffer(const void *data, unsigned int size)
{
    // Create buffer
    GLCALL(glGenBuffers(1, &id_));
    GLCALL(glBindBuffer(GL_ARRAY_BUFFER, id_));

    // Put data into buffers (target, size in bytes, *data, usage)
    GLCALL(glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW));
}

VertexBuffer::~VertexBuffer()
{
    GLCALL(glDeleteBuffers(1, &id_));
}

void VertexBuffer::Bind() const
{
    GLCALL(glBindBuffer(GL_ARRAY_BUFFER, id_));
}

void VertexBuffer::Unbind() const
{
    GLCALL(glBindBuffer(GL_ARRAY_BUFFER, 0));
}
