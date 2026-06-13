#pragma once

#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstdlib>

struct vec3 {
    float x;
    float y;
    float z;
};

inline const vec3 operator+(const vec3& a, const vec3& b) {
    return {a.x + b.x, a.y + b.y, a.z + b.z};
}
inline const vec3 operator-(const vec3& a, const vec3& b) {
    return {a.x - b.x, a.y - b.y, a.z - b.z};
}
inline const vec3 operator*(const vec3& a, float b) {
    return {a.x * b, a.y * b, a.z * b};
}
inline const vec3 operator/(const vec3& a, float b) {
    return {a.x / b, a.y / b, a.z / b};
}
inline const vec3 operator*(float a, const vec3& b) {
    return {a * b.x, a * b.y, a * b.z};
}
inline const vec3 operator/(float a, const vec3& b) {
    return {a / b.x, a / b.y, a / b.z};
}
inline const vec3 operator-(const vec3& a) {
    return {-a.x, -a.y, -a.z};
}
inline const vec3 operator*(const vec3& a, const vec3& b) {
    return {a.x * b.x, a.y * b.y, a.z * b.z};
}
inline const vec3 operator/(const vec3& a, const vec3& b) {
    return {a.x / b.x, a.y / b.y, a.z / b.z};
}
inline const vec3 operator+(const vec3& a) {
    return {a.x, a.y, a.z};
}

inline float vec3_dot(const vec3& a, const vec3& b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

inline vec3 vec3_cross(const vec3& a, const vec3& b) {
    return {
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x,
    };
}

inline float vec3_length_sq(const vec3& v) {
    return vec3_dot(v, v);
}

inline float vec3_length(const vec3& v) {
    return std::sqrt(vec3_length_sq(v));
}

inline vec3 vec3_normalize(const vec3& v) {
    const float len = vec3_length(v);
    if (len <= 0.0f) {
        return {0.0f, 0.0f, 0.0f};
    }
    return v / len;
}

inline vec3 vec3_lerp(const vec3& a, const vec3& b, float t) {
    return a + (b - a) * t;
}
