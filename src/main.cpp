#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/rotate_vector.hpp>

#include <iostream>
#include <vector>
#include <fstream>
#include <algorithm>

#include "renderer.h"
#include "vertex_buffer.h"
#include "shader.h"
#include "index_buffer.h"
#include "vertex_buffer_layout.h"
#include "vertex_array.h"
#include "renderer.h"
#include "input.h"

struct Vertex
{
    float position[2];
};

int main(void)
{
    // Initialize the GLFW library
    if (!glfwInit())
        return -1;

    // specify the client API version that the created context must be compatible with
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);

    /* 
    specifies whether the OpenGL context should be forward-compatible, i.e. one
    where all functionality deprecated in the requested version  of OpenGL is 
    removed. This must only be used if the requested OpenGL version is 3.0 or above.
    */
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    // Create a windowed mode window and its OpenGL context
    const int WINDOW_WIDTH = 800;
    const int WINDOW_HEIGHT = 800;
    const char* WINDOW_TITLE = "Drift Simluation";
    GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE, NULL, NULL);
    if (!window) // Error
    {
        std::cout << "Window create error" << std::endl;
        glfwTerminate();
        return -1;
    }

    /* Make the OpenGL context of the specified window current
        An OpenGL context represents many things. A context stores all of the state 
        associated with this instance of OpenGL. It represents the (potentially visible) 
        default framebuffer that rendering commands will draw to when not drawing to a 
        framebuffer object. Think of a context as an object that holds all of OpenGL; 
        when a context is destroyed, OpenGL is destroyed.
     */
    glfwMakeContextCurrent(window);

    glfwSetKeyCallback(window, Input::KeyCallback);

	// Min number of screen updates to wait for until the buffers are swapped by glfwSwapBuffers
    glfwSwapInterval(1);

    // Create OpenGL rendering context before calling glewInit
    // Initialize the extension entry points
    if(glewInit() != GLEW_OK)
        std::cout << "GLEW ERROR!" << std::endl;

    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    const int N_ROWS = 25;
    const int N_COLS = 25;

    float row_offset = static_cast<float>(WINDOW_HEIGHT) / N_ROWS;
    float col_offset = static_cast<float>(WINDOW_WIDTH) / N_COLS;

    for(int row = 0;row<N_ROWS;row++)
    {
        for(int col = 0;col<N_COLS;col++)
        {
            vertices.push_back({{
                static_cast<float>(col*col_offset + col_offset/2),
                static_cast<float>(row*row_offset + row_offset/2),
            }});
            indices.push_back(vertices.size()-1);
        }
    }

    VertexArray va;
    VertexBuffer vb(&vertices[0], vertices.size() * sizeof(Vertex));

    VertexBufferLayout layout;
    layout.Push<float>(2); // position

    IndexBuffer ib(&indices[0], indices.size());

    va.AddBuffer(vb, layout, ib);

    // Create and use shader program
    // *** Relative file paths are relative to current working directory ***
    Shader shader(
        "/Users/michael/Documents/projects/drift-simulation-opengl/res/shaders/vertex.shader", 
        "/Users/michael/Documents/projects/drift-simulation-opengl/res/shaders/geometry.shader", 
        "/Users/michael/Documents/projects/drift-simulation-opengl/res/shaders/fragment.shader");
    shader.Bind();

    Renderer renderer;

    glm::mat4 proj_mat = glm::ortho(0.0f, static_cast<float>(WINDOW_WIDTH), 0.0f, static_cast<float>(WINDOW_HEIGHT));
    shader.SetUniformMatrix4f("proj_mat", proj_mat);
    
    float angle = 0.0f;
    float length = 15.0f;
    float width = 1.0f;

    // Loop until the user closes the window
    while (!glfwWindowShouldClose(window) && !Input::EscapePressed())
    {
        // Poll for and process events
        glfwPollEvents();

        renderer.Clear();

        if(Input::LeftPressed()) angle += 0.05f;
        if(Input::RightPressed()) angle -= 0.05f;
        if(Input::UpPressed()) length += 0.5f;
        if(Input::DownPressed()) length = std::max(0.0f, length - 0.5f);

        if(Input::APressed()) width = std::max(0.0f, width - 0.1f);
        if(Input::DPressed()) width += 0.1f;


        glm::mat rot_mat = glm::rotate(angle, glm::vec3(0.0f,0.0f,1.0f));
        shader.SetUniformMatrix4f("rot_mat", rot_mat);
        shader.SetUniform1f("length", length);
        shader.SetUniform1f("width", width);

        renderer.Draw(GL_POINTS, va, shader);

        // Swap front and back buffers
        glfwSwapBuffers(window);
    }

    // Destroys all remaining windows and cursors and frees allocated resources
    glfwTerminate();
    return 0;
}