#version 410 core

layout (points) in;
layout (triangle_strip, max_vertices = 4) out;

out GS_OUT
{
    vec3 color;
} gs_out;

uniform mat4 proj_mat;
uniform float length;
uniform float width;
uniform mat4 rot_mat;

void main()
{
    vec3 base_point = gl_in[0].gl_Position.xyz;
    vec3 tip_point = base_point + (rot_mat * vec4(length, 0.0, 0.0, 0.0)).xyz;

    vec3 forward = normalize(tip_point - base_point);
    vec3 right = cross(forward, vec3(0.0,0.0,1.0));

    vec3 base_right = base_point + right * width / 2.0;
    vec3 tip_right = base_right + forward * length;

    vec3 base_left = base_point - right * width / 2;
    vec3 tip_left = base_left + forward * length;

    gs_out.color = vec3(0.0,1.0,0.0);

    gl_Position = proj_mat * vec4(base_left,1.0);
    EmitVertex();

    gl_Position = proj_mat * vec4(tip_left,1.0);
    EmitVertex();

    gl_Position = proj_mat * vec4(base_right,1.0);
    EmitVertex();

    gl_Position = proj_mat * vec4(tip_right,1.0);
    EmitVertex();
}