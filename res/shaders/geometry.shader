#version 410 core

layout (points) in;
layout (triangle_strip, max_vertices = 64) out;

in VS_OUT
{
    float angle_noise;
    float size_noise;
} gs_in [];

out GS_OUT
{
    vec4 color;
} gs_out;

uniform mat4 proj_mat;
uniform float hue_drift;

#define PI 3.1415926538

uniform float angle_scale;
uniform float angle_z;
uniform float size_scale;
uniform float size_z;


// ******************** PERLIN NOISE ********************

uniform uint seed;

// https://stackoverflow.com/a/12996028
uint Hash(uint x)
{
    x = ((x >> 16) ^ x) * 0x45d9f3b;
    x = ((x >> 16) ^ x) * 0x45d9f3b;
    x = (x >> 16) ^ x;
    return x;
}

// https://stackoverflow.com/a/27952689
uint HashCombine(uint lhs, uint rhs)
{
    lhs ^= rhs + 0x9e3779b9 + (lhs << 6) + (lhs >> 2);
    return lhs;
}

vec3 RandomGradient(int xi, int yi, int zi)
{
    uint h = HashCombine(HashCombine(HashCombine(Hash(xi), Hash(yi)), Hash(zi)), Hash(seed));

    vec3 grads[12] = vec3[](
        normalize(vec3( 1.0f, 0.0f, 1.0f)),
        normalize(vec3(-1.0f, 0.0f, 1.0f)),
        normalize(vec3( 0.0f, 1.0f, 1.0f)),
        normalize(vec3( 0.0f,-1.0f, 1.0f)),

        normalize(vec3( 1.0f, 0.0f,-1.0f)),
        normalize(vec3(-1.0f, 0.0f,-1.0f)),
        normalize(vec3( 0.0f, 1.0f,-1.0f)),
        normalize(vec3( 0.0f,-1.0f,-1.0f)),

        normalize(vec3( 1.0f, 1.0f, 0.0f)),
        normalize(vec3(-1.0f, 1.0f, 0.0f)),
        normalize(vec3( 1.0f,-1.0f, 0.0f)),
        normalize(vec3(-1.0f,-1.0f, 0.0f))
    );

    return grads[h % 12];
}

float DotGridGradient(float x, float y, float z, int xi, int yi, int zi)
{
    vec3 g = RandomGradient(xi,yi,zi);
    float dx = x - xi, dy = y - yi, dz = z - zi;
    return (g.x * dx) + (g.y * dy) + (g.z * dz);
}

float SmootherStep(float x)
{
    return x * x * x * (x * (x * 6 - 15) + 10);
}

float PerlinNoise(float x, float y, float z)
{
        int x0 = int(floor(x));
        int x1 = x0 + 1;
        int y0 = int(floor(y));
        int y1 = y0 + 1;
        int z0 = int(floor(z));
        int z1 = z0 + 1;

        float dx = x - x0;
        float dy = y - y0;
        float dz = z - z0;

        dx = SmootherStep(dx);
        dy = SmootherStep(dy);
        dz = SmootherStep(dz);

        float p0 = DotGridGradient(x, y, z, x0, y0, z0);
        float p1 = DotGridGradient(x, y, z, x1, y0, z0);
        float l0 = mix(p0, p1, dx);

        p0 = DotGridGradient(x, y, z, x0, y1, z0);
        p1 = DotGridGradient(x, y, z, x1, y1, z0);
        float l1 = mix(p0, p1, dx);

        float f0 = mix(l0, l1, dy);

        p0 = DotGridGradient(x, y, z, x0, y0, z1);
        p1 = DotGridGradient(x, y, z, x1, y0, z1);
        l0 = mix(p0, p1, dx);

        p0 = DotGridGradient(x, y, z, x0, y1, z1);
        p1 = DotGridGradient(x, y, z, x1, y1, z1);
        l1 = mix(p0, p1, dx);

        float f1 = mix(l0, l1, dy);

        // multiply by const factor to bring into range [-1,1]
        return mix(f0, f1, dz) * 1.6f;
}

// ******************** PERLIN NOISE ********************


// All components are in the range [0,1], including hue.
vec3 hsv2rgb(vec3 c)
{
    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

void main()
{
    // float angle_noise = gs_in[0].angle_noise / 2.0 + 0.5;
    float angle_noise = PerlinNoise(
        gl_in[0].gl_Position.x * angle_scale, 
        gl_in[0].gl_Position.y * angle_scale, 
        angle_z
    ) / 2.0 + 0.5;

    float theta = angle_noise * PI*4;

    // float size_noise = gs_in[0].size_noise / 2.0 + 0.5;
    float size_noise = PerlinNoise(
        gl_in[0].gl_Position.x * size_scale, 
        gl_in[0].gl_Position.y * size_scale, 
        size_z
    ) / 2.0 + 0.5;

    float length = max(0.0, pow(size_noise, 3.0) * 225.0 - 10.0);
    float width = min(20.0, max(0.0, length / 3.5));

    length *= 35.0/50.0;
    width *= 35.0/50.0;

    vec3 base_point = gl_in[0].gl_Position.xyz;
    vec3 tip_point = base_point + length * vec3(cos(theta), sin(theta), 0.0);
    tip_point.z += 0.5;

    vec3 forward = normalize(tip_point - base_point);
    vec3 right = cross(forward, vec3(0.0, 0.0, 1.0));

    vec3 base_right = base_point + right * width / 2.0;
    vec3 tip_right = base_right + forward * length;

    vec3 base_left = base_point - right * width / 2.0;
    vec3 tip_left = base_left + forward * length;

    float hue = angle_noise / 2.0 + hue_drift;
    hue = hue - int(hue);

    vec3 color = hsv2rgb(vec3(hue, 1.0, 0.8));

    float tip_alpha = 0.75f;
    float base_alpha = 0.0f;

    gl_Position = proj_mat * vec4(base_right, 1.0);
    gs_out.color = vec4(color, base_alpha);
    EmitVertex();

    gl_Position = proj_mat * vec4(base_left, 1.0);
    gs_out.color = vec4(color, base_alpha);
    EmitVertex();

    gl_Position = proj_mat * vec4(tip_right, 1.0);
    gs_out.color = vec4(color, tip_alpha);
    EmitVertex();

    gl_Position = proj_mat * vec4(tip_left, 1.0);
    gs_out.color = vec4(color, tip_alpha);
    EmitVertex();

    vec3 center = (tip_left + tip_right) / 2.0;
    vec3 p_left = tip_left - center;
    vec3 p_right = tip_right - center;

    const int N_POINTS = 16;
    const float t = PI / (N_POINTS+1);
    // column major order
    mat3 rot_ccw = mat3(
         cos(t), sin(t), 0,
        -sin(t), cos(t), 0,
              0,      0, 1
    );
    mat3 rot_cw = mat3(
         cos(t), -sin(t), 0,
        sin(t), cos(t), 0,
              0,      0, 1
    );

    for(int i = 0; i < N_POINTS; i++)
    {
        if(i % 2 == 0) // right
        {
            p_right = rot_ccw * p_right;
            gl_Position = proj_mat * vec4(p_right + center, 1.0);
        }
        else // left
        {
            p_left = rot_cw * p_left;
            gl_Position = proj_mat * vec4(p_left + center, 1.0);
        }
        gs_out.color = vec4(color, tip_alpha);
        EmitVertex();
    }
}
