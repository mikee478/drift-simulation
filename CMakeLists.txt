cmake_minimum_required(VERSION 3.24.2) # Require a minimum version of cmake
project(drift-simulation) # Set the name of the project
set(CMAKE_CXX_STANDARD 20) # Enable c++20 standard

# Add executable target
add_executable(${PROJECT_NAME})

find_package(glfw3 3.3 REQUIRED)
target_link_libraries(${PROJECT_NAME} glfw)

find_package(OpenGL REQUIRED)
target_link_libraries(${PROJECT_NAME} OpenGL::GL)

find_package(GLEW REQUIRED)
target_link_libraries(${PROJECT_NAME} GLEW::GLEW)

# Add include directories to a target
target_include_directories(${PROJECT_NAME} 
PRIVATE 
    ${PROJECT_SOURCE_DIR}/src
    /opt/homebrew/include # glm
)

# Add sources to a target
target_sources(${PROJECT_NAME} 
PRIVATE 
    src/main.cpp
    src/renderer.cpp
    src/vertex_buffer.cpp
    src/shader.cpp
    src/index_buffer.cpp
    src/vertex_array.cpp

    src/imgui/imgui_demo.cpp
    src/imgui/imgui_draw.cpp
    src/imgui/imgui_impl_glfw.cpp
    src/imgui/imgui_impl_opengl3.cpp
    src/imgui/imgui_tables.cpp
    src/imgui/imgui_widgets.cpp
    src/imgui/imgui.cpp
)

target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra)
