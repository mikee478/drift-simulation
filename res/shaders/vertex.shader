#version 410 core

layout(location = 0) in vec2 position;
layout(location = 1) in float angle_noise;
layout(location = 2) in float size_noise;

out VS_OUT
{
    float angle_noise;
    float size_noise;
} vs_out;

void main()
{
    gl_Position = vec4(position, 0.0, 1.0);
    vs_out.angle_noise = angle_noise;
    vs_out.size_noise = size_noise;
}