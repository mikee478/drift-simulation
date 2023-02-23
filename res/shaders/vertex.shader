#version 410 core

layout(location = 0) in vec4 position;

// out VS_OUT
// {
//     vec3 position;
// } vs_out;

uniform mat4 proj_mat;

void main()
{
    gl_Position = position;
    gl_PointSize = 10.0;
}