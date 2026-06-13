#pragma once

#include <cmath>

#include "abs/quaternion.hpp"
#include "abs/vec3.hpp"

inline float abs_epsilon() {
    return 1e-6f;
}

inline bool almost_equal(float a, float b, float epsilon = 1e-6f) {
    return std::fabs(a - b) <= epsilon;
}

inline bool almost_zero(float x, float epsilon = 1e-6f) {
    return std::fabs(x) <= epsilon;
}

inline float clampf(float x, float lo, float hi) {
    return x < lo ? lo : (x > hi ? hi : x);
}

inline float saturate(float x) {
    return clampf(x, 0.0f, 1.0f);
}

inline float wrap_angle_pi(float radians) {
    const float two_pi = 6.28318530718f;
    float v = std::fmod(radians + 3.14159265359f, two_pi);
    if (v < 0.0f) {
        v += two_pi;
    }
    return v - 3.14159265359f;
}

inline float minf(float a, float b) {
    return a < b ? a : b;
}

inline float maxf(float a, float b) {
    return a > b ? a : b;
}

inline vec3 vec3_safe_normalize(const vec3& v, float epsilon = 1e-8f) {
    const float len_sq = vec3_length_sq(v);
    if (len_sq <= epsilon) {
        return {0.0f, 0.0f, 0.0f};
    }
    return v / std::sqrt(len_sq);
}

inline quat quat_safe_normalize(const quat& q, float epsilon = 1e-8f) {
    const float len_sq = quat_length_sq(q);
    if (len_sq <= epsilon) {
        return quat_identity();
    }
    return q / std::sqrt(len_sq);
}
