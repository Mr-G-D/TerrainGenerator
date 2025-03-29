#include <iostream>
#include <vector>
#include <cmath>

// Perlin noise helper functions
float fade(float t) {
    return t * t * t * (t * (t * 6 - 15) + 10);
}

float lerp(float t, float a, float b) {
    return a + t * (b - a);
}

float grad(int hash, float x, float y) {
    int h = hash & 15;
    float u = h < 8 ? x : y;
    float v = h < 4 ? y : (h == 12 || h == 14 ? x : 0);
    return (h & 1 ? -u : u) + (h & 2 ? -v : v);
}

int p[512];

// Perlin noise functions
float perlin_noise(float x, float y, int seed) {
    int X = static_cast<int>(floor(x)) & 255;
    int Y = static_cast<int>(floor(y)) & 255;
    int Z = seed & 255;
    x -= floor(x);
    y -= floor(y);
    float u = fade(x);
    float v = fade(y);
    int A = p[X] + Y;
    int AA = p[A] + Z;
    int AB = p[A + 1] + Z;
    int B = p[X + 1] + Y;
    int BA = p[B] + Z;
    int BB = p[B + 1] + Z;
    return lerp(v, lerp(u, grad(p[AA], x, y), grad(p[BA], x - 1, y)),
        lerp(u, grad(p[AB], x, y - 1), grad(p[BB], x - 1, y - 1)));
}

// Initialize the permutation table (randomize it)
void init_permutation(int seed) {
    for (int i = 0; i < 256; i++) {
        p[i] = i;
    }
    std::srand(seed);
    for (int i = 0; i < 256; i++) {
        int j = std::rand() % 256;
        std::swap(p[i], p[j]);
        p[i + 256] = p[i];
    }
}
