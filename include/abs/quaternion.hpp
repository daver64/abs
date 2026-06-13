#pragma once

#include <cmath>

#include "abs/vec3.hpp"

struct quat {
    float w;
    float x;
    float y;
    float z;
};

inline const quat operator+(const quat& a, const quat& b) {
    return {a.w + b.w, a.x + b.x, a.y + b.y, a.z + b.z};
}

inline const quat operator-(const quat& a, const quat& b) {
    return {a.w - b.w, a.x - b.x, a.y - b.y, a.z - b.z};
}

inline const quat operator*(const quat& q, float s) {
    return {q.w * s, q.x * s, q.y * s, q.z * s};
}

inline const quat operator*(float s, const quat& q) {
    return q * s;
}

inline const quat operator/(const quat& q, float s) {
    return {q.w / s, q.x / s, q.y / s, q.z / s};
}

inline const quat operator*(const quat& a, const quat& b) {
    return {
        a.w * b.w - a.x * b.x - a.y * b.y - a.z * b.z,
        a.w * b.x + a.x * b.w + a.y * b.z - a.z * b.y,
        a.w * b.y - a.x * b.z + a.y * b.w + a.z * b.x,
        a.w * b.z + a.x * b.y - a.y * b.x + a.z * b.w,
    };
}

inline quat quat_identity() {
    return {1.0f, 0.0f, 0.0f, 0.0f};
}

inline quat quat_conjugate(const quat& q) {
    return {q.w, -q.x, -q.y, -q.z};
}

inline float quat_dot(const quat& a, const quat& b) {
    return a.w * b.w + a.x * b.x + a.y * b.y + a.z * b.z;
}

inline float quat_length_sq(const quat& q) {
    return quat_dot(q, q);
}

inline float quat_length(const quat& q) {
    return std::sqrt(quat_length_sq(q));
}

inline quat quat_normalize(const quat& q) {
    const float len = quat_length(q);
    if (len <= 0.0f) {
        return quat_identity();
    }
    return q / len;
}

inline quat quat_inverse(const quat& q) {
    const float len_sq = quat_length_sq(q);
    if (len_sq <= 0.0f) {
        return quat_identity();
    }
    return quat_conjugate(q) / len_sq;
}

inline quat quat_from_axis_angle(const vec3& axis, float angle_radians) {
    const vec3 n = vec3_normalize(axis);
    const float half = 0.5f * angle_radians;
    const float s = std::sin(half);
    const float c = std::cos(half);
    return {c, n.x * s, n.y * s, n.z * s};
}

inline vec3 quat_rotate_vec3(const quat& q, const vec3& v) {
    const quat qn = quat_normalize(q);
    const quat p{0.0f, v.x, v.y, v.z};
    const quat r = qn * p * quat_conjugate(qn);
    return {r.x, r.y, r.z};
}
