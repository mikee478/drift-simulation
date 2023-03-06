#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/random.hpp>

#include <iostream>
#include <vector>

#include "renderer.h"
#include "vertex_buffer.h"
#include "shader.h"
#include "index_buffer.h"
#include "vertex_buffer_layout.h"
#include "vertex_array.h"
#include "renderer.h"
#include "input.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

// #define IMGUI_DEBUG_WINDOW

int main(void)
{
    if (!glfwInit())
        return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    const int WINDOW_WIDTH = 800;
    const int WINDOW_HEIGHT = 800;
    const char* WINDOW_TITLE = "Drift Simluation";
    GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE, NULL, NULL);
    if (!window)
    {
        std::cout << "Window create error" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, Input::KeyCallback);
    glfwSwapInterval(1);

    if(glewInit() != GLEW_OK)
    {
        std::cout << "GLEW ERROR!" << std::endl;
        return -1;
    }

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

    const int N_ROWS = 60;
    const int N_COLS = 60;

    const float row_offset = static_cast<float>(WINDOW_HEIGHT) / (N_ROWS-1);
    const float col_offset = static_cast<float>(WINDOW_WIDTH) / (N_COLS-1);

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
    shader.SetUniform1ui("seed", glm::linearRand(0u, 1000000u));

    float angle_z = 0.0f;
    float size_z = 0.0f;
    float hue_drift = glm::linearRand(0.0f, 1.0f);
    float angle_scale = 0.00175f;
    float size_scale = 0.00175f;

    float angle_z_delta = 0.004f;
    float size_z_delta = 0.004f;
    float hue_drift_delta = 0.0005f;

    double start_time = glfwGetTime();
    double fps;
    int n_frames = 0;

    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    const char* glsl_version = "#version 150";
    ImGui_ImplOpenGL3_Init(glsl_version);

    while (!glfwWindowShouldClose(window) && !Input::EscapePressed())
    {
        glfwPollEvents();

        renderer.Clear();

        #ifdef IMGUI_DEBUG_WINDOW
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("Debug Window");
        ImGui::SliderFloat("angle_z_delta", &angle_z_delta, 0.0f, 0.01f);
        ImGui::SliderFloat("size_z_delta", &size_z_delta, 0.0f, 0.01f);
        ImGui::SliderFloat("hue_drift_delta", &hue_drift_delta, 0.0f, 0.01f);
        ImGui::SliderFloat("angle_scale", &angle_scale, 0.0f, 0.003f);
        ImGui::SliderFloat("size_scale", &size_scale, 0.0f, 0.003f);
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        ImGui::End();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        #endif

        angle_z += angle_z_delta;
        size_z += size_z_delta;
        hue_drift += hue_drift_delta;

        shader.SetUniform1f("angle_z", angle_z);
        shader.SetUniform1f("size_z", size_z);
        shader.SetUniform1f("hue_drift", hue_drift);
        shader.SetUniform1f("angle_scale", angle_scale);
        shader.SetUniform1f("size_scale", size_scale);

        renderer.Draw(GL_POINTS, va, shader);

        glfwSwapBuffers(window);

        if(++n_frames % 30 == 0)
        {
            fps = n_frames / (glfwGetTime() - start_time);
            std::cout << "fps = " << std::round(fps) << '\n';
        }
        
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwTerminate();
    return 0;
}