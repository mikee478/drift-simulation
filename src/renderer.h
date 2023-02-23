#pragma once

#include <iostream>
#include "vertex_array.h"
#include "shader.h"

#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)

#define GLCALL(x) GLClearErrors();\
    x;\
    if(!GLLogCall(#x, __FILENAME__, __LINE__)) {std::cout << "\n*** EXECUTION SUSPENDED ***" << std::endl; std::cin.get(); }

void GLClearErrors();
bool GLLogCall(const char* function, const char* file, int line);

class Renderer
{
public:
    Renderer();
    void Clear() const;
    void Draw(unsigned int mode, const VertexArray& va, const Shader& shader) const;
};