class PerlinNoise
{
public:
    PerlinNoise(uint32_t seed) : seed_(Hash(seed)) {}

    float operator()(float x, float y, float z)
    {
        int x0 = static_cast<int>(floor(x));
        int x1 = x0 + 1;
        int y0 = static_cast<int>(floor(y));
        int y1 = y0 + 1;
        int z0 = static_cast<int>(floor(z));
        int z1 = z0 + 1;

        float dx = x - static_cast<float>(x0);
        float dy = y - static_cast<float>(y0);
        float dz = z - static_cast<float>(z0);

        dx = SmootherStep(dx);
        dy = SmootherStep(dy);
        dz = SmootherStep(dz);

        float p0 = DotGridGradient(x, y, z, x0, y0, z0);
        float p1 = DotGridGradient(x, y, z, x1, y0, z0);
        float l0 = Lerp(p0, p1, dx);

        p0 = DotGridGradient(x, y, z, x0, y1, z0);
        p1 = DotGridGradient(x, y, z, x1, y1, z0);
        float l1 = Lerp(p0, p1, dx);

        float f0 = Lerp(l0, l1, dy);

        p0 = DotGridGradient(x, y, z, x0, y0, z1);
        p1 = DotGridGradient(x, y, z, x1, y0, z1);
        l0 = Lerp(p0, p1, dx);

        p0 = DotGridGradient(x, y, z, x0, y1, z1);
        p1 = DotGridGradient(x, y, z, x1, y1, z1);
        l1 = Lerp(p0, p1, dx);

        float f1 = Lerp(l0, l1, dy);

        // multiply by const factor to bring into range [-1,1]
        return Lerp(f0, f1, dz) * 1.6f;
    }

private:
    float SmootherStep(float x)
    {
        return x * x * x * (x * (x * 6 - 15) + 10);
    }

    float Lerp(float a, float b, float x) 
    { 
        return a + (b - a) * x;
    }

    // https://stackoverflow.com/a/27952689
    uint32_t HashCombine(uint32_t lhs, uint32_t rhs) 
    {
        lhs ^= rhs + 0x9e3779b9 + (lhs << 6) + (lhs >> 2);
        return lhs;
    }

    // https://stackoverflow.com/a/12996028
    uint32_t Hash(uint32_t x)
    {
        x = ((x >> 16) ^ x) * 0x45d9f3b;
        x = ((x >> 16) ^ x) * 0x45d9f3b;
        x = (x >> 16) ^ x;
        return x;
    }

    glm::vec3 RandomGradient(int xi, int yi, int zi) 
    {
        uint32_t h = HashCombine(HashCombine(HashCombine(Hash(xi), Hash(yi)), Hash(zi)), Hash(seed_));

        static const glm::vec3 grads[12] = {
            glm::normalize(glm::vec3( 1.0f, 0.0f, 1.0f)),
            glm::normalize(glm::vec3(-1.0f, 0.0f, 1.0f)),
            glm::normalize(glm::vec3( 0.0f, 1.0f, 1.0f)),
            glm::normalize(glm::vec3( 0.0f,-1.0f, 1.0f)),

            glm::normalize(glm::vec3( 1.0f, 0.0f,-1.0f)),
            glm::normalize(glm::vec3(-1.0f, 0.0f,-1.0f)),
            glm::normalize(glm::vec3( 0.0f, 1.0f,-1.0f)),
            glm::normalize(glm::vec3( 0.0f,-1.0f,-1.0f)),

            glm::normalize(glm::vec3( 1.0f, 1.0f, 0.0f)),
            glm::normalize(glm::vec3(-1.0f, 1.0f, 0.0f)),
            glm::normalize(glm::vec3( 1.0f,-1.0f, 0.0f)),
            glm::normalize(glm::vec3(-1.0f,-1.0f, 0.0f)),
        };

        return grads[h % 12];
    }

    float DotGridGradient(float x, float y, float z, int xi, int yi, int zi) 
    {
        glm::vec3 g = RandomGradient(xi,yi,zi);
        float dx = x - xi, dy = y - yi, dz = z - zi;
        return (g.x * dx) + (g.y * dy) + (g.z * dz);
    }

    uint32_t seed_;
};