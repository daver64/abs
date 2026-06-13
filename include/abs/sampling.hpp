#pragma once

#include <cmath>
#include <cstdint>

#include "abs/vec3.hpp"

inline float fractf(float x) {
    return x - std::floor(x);
}

inline float radical_inverse_vdc(uint32_t bits) {
    bits = (bits << 16u) | (bits >> 16u);
    bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
    bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
    bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
    bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
    return static_cast<float>(bits) * 2.3283064365386963e-10f;
}

inline vec3 hammersley_2d(uint32_t i, uint32_t n) {
    return {static_cast<float>(i) / static_cast<float>(n), radical_inverse_vdc(i), 0.0f};
}

inline float halton(uint32_t index, uint32_t base) {
    float f = 1.0f;
    float r = 0.0f;
    while (index > 0u) {
        f /= static_cast<float>(base);
        r += f * static_cast<float>(index % base);
        index /= base;
    }
    return r;
}

inline vec3 sample_uniform_sphere(float u1, float u2) {
    const float z = 1.0f - 2.0f * u1;
    const float r = std::sqrt(1.0f - z * z);
    const float phi = 6.28318530718f * u2;
    return {r * std::cos(phi), r * std::sin(phi), z};
}

inline vec3 sample_uniform_hemisphere(float u1, float u2) {
    const float z = u1;
    const float r = std::sqrt(1.0f - z * z);
    const float phi = 6.28318530718f * u2;
    return {r * std::cos(phi), r * std::sin(phi), z};
}

inline vec3 sample_cosine_hemisphere(float u1, float u2) {
    const float r = std::sqrt(u1);
    const float phi = 6.28318530718f * u2;
    const float x = r * std::cos(phi);
    const float y = r * std::sin(phi);
    const float z = std::sqrt(1.0f - u1);
    return {x, y, z};
}

inline vec3 sample_uniform_disk(float u1, float u2) {
    const float r = std::sqrt(u1);
    const float phi = 6.28318530718f * u2;
    return {r * std::cos(phi), r * std::sin(phi), 0.0f};
}
