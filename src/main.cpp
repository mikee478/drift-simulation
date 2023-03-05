#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/random.hpp>
#include <glm/gtc/noise.hpp>

#include <iostream>
#include <vector>
#include <algorithm>

#include "renderer.h"
#include "vertex_buffer.h"
#include "shader.h"
#include "index_buffer.h"
#include "vertex_buffer_layout.h"
#include "vertex_array.h"
#include "renderer.h"
#include "input.h"
#include "noise.h"

int main(void)
{
    if (!glfwInit())
        return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);

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

    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, Input::KeyCallback);
    glfwSwapInterval(1);

    if(glewInit() != GLEW_OK)
        std::cout << "GLEW ERROR!" << std::endl;

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    struct Vertex
    {
        float position[2];
    };

    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    const int N_OUTSIDE_ROWS = 5;
    const int N_OUTSIDE_COLS = 5;

    const int N_ROWS = 50;
    const int N_COLS = 50;

    float row_offset = static_cast<float>(WINDOW_HEIGHT) / (N_ROWS-1);
    float col_offset = static_cast<float>(WINDOW_WIDTH) / (N_COLS-1);

    for(int row = -N_OUTSIDE_ROWS; row < N_ROWS + N_OUTSIDE_ROWS; row++)
    {
        for(int col = -N_OUTSIDE_COLS; col < N_COLS + N_OUTSIDE_COLS; col++)
        {
            vertices.push_back({{col * col_offset, row * row_offset}});
            indices.push_back(vertices.size()-1);
        }
    }

    VertexArray va;
    VertexBuffer vb(&vertices[0], vertices.size() * sizeof(Vertex), GL_DYNAMIC_DRAW);

    VertexBufferLayout layout;
    layout.Push<float>(2); // position

    IndexBuffer ib(&indices[0], indices.size());

    va.AddBuffer(vb, layout, ib);

    Shader shader(
        "/Users/michael/Documents/projects/drift-simulation-opengl/res/shaders/vertex.shader", 
        "/Users/michael/Documents/projects/drift-simulation-opengl/res/shaders/geometry.shader", 
        "/Users/michael/Documents/projects/drift-simulation-opengl/res/shaders/fragment.shader");
    shader.Bind();

    Renderer renderer;

    glm::mat4 proj_mat = glm::ortho(0.0f, static_cast<float>(WINDOW_WIDTH), 0.0f, static_cast<float>(WINDOW_HEIGHT));
    shader.SetUniformMatrix4f("proj_mat", proj_mat);

    srand(time(0));
    float angle_z = glm::linearRand(-10000.0f, 10000.0f);
    float size_z = glm::linearRand(-10000.0f, 10000.0f);

    float hue_drift = glm::linearRand(0.0f, 1.0f);
    float f_angle = 0.0015f;
    float f_size = 0.00175f;

    shader.SetUniform1f("angle_scale", f_angle);
    shader.SetUniform1f("size_scale", f_size);

    double start_time = glfwGetTime();
    double cur_time, fps;
    int n_frames = 0;

    // Loop until the user closes the window
    while (!glfwWindowShouldClose(window) && !Input::EscapePressed())
    {
        // Poll for and process events
        glfwPollEvents();

        renderer.Clear();

        angle_z += 0.0025f * 1.25f;
        size_z += 0.003f * 1.25;
        hue_drift += 0.0005f;

        shader.SetUniform1f("angle_z", angle_z);
        shader.SetUniform1f("size_z", size_z);
        shader.SetUniform1f("hue_drift", hue_drift);

        renderer.Draw(GL_POINTS, va, shader);

        glfwSwapBuffers(window);

        cur_time = glfwGetTime();
        n_frames++;
        if(n_frames % 10 == 0)
        {
            fps = n_frames / (cur_time - start_time);
            std::cout << "fps = " << std::round(fps) << '\n';
        }
        
    }

    glfwTerminate();
    return 0;
}