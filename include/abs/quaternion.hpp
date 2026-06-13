#pragma once

#include <algorithm>
#include <cmath>

#include "abs/vec3.hpp"

struct quat {
    float w;
    float x;
    float y;
    float z;
};

enum euler_order {
    EULER_ORDER_XYZ,
    EULER_ORDER_XZY,
    EULER_ORDER_YXZ,
    EULER_ORDER_YZX,
    EULER_ORDER_ZXY,
    EULER_ORDER_ZYX,
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

inline quat quat_nlerp(const quat& a, const quat& b, float t, bool shortest_path = true) {
    quat end = b;
    if (shortest_path && quat_dot(a, b) < 0.0f) {
        end = {-b.w, -b.x, -b.y, -b.z};
    }
    return quat_normalize(a * (1.0f - t) + end * t);
}

inline quat quat_slerp(const quat& a, const quat& b, float t, bool shortest_path = true) {
    quat end = b;
    float cos_theta = quat_dot(a, b);

    if (shortest_path && cos_theta < 0.0f) {
        end = {-b.w, -b.x, -b.y, -b.z};
        cos_theta = -cos_theta;
    }

    if (cos_theta > 0.9995f) {
        return quat_nlerp(a, end, t, false);
    }

    cos_theta = std::clamp(cos_theta, -1.0f, 1.0f);
    const float theta = std::acos(cos_theta);
    const float sin_theta = std::sin(theta);
    if (sin_theta <= 0.0f) {
        return quat_nlerp(a, end, t, false);
    }

    const float wa = std::sin((1.0f - t) * theta) / sin_theta;
    const float wb = std::sin(t * theta) / sin_theta;
    return quat_normalize(a * wa + end * wb);
}

inline quat quat_from_two_vectors(const vec3& from, const vec3& to) {
    const vec3 f = vec3_normalize(from);
    const vec3 t = vec3_normalize(to);
    const float d = vec3_dot(f, t);

    if (d >= 1.0f - 1e-6f) {
        return quat_identity();
    }

    if (d <= -1.0f + 1e-6f) {
        const vec3 axis = vec3_normalize(std::fabs(f.x) < 0.9f ? vec3_cross(f, {1.0f, 0.0f, 0.0f})
                                                                : vec3_cross(f, {0.0f, 1.0f, 0.0f}));
        return quat_from_axis_angle(axis, 3.14159265359f);
    }

    const vec3 c = vec3_cross(f, t);
    const quat q{1.0f + d, c.x, c.y, c.z};
    return quat_normalize(q);
}

inline quat quat_from_euler_xyz(const vec3& angles_radians) {
    const float cx = std::cos(angles_radians.x * 0.5f);
    const float sx = std::sin(angles_radians.x * 0.5f);
    const float cy = std::cos(angles_radians.y * 0.5f);
    const float sy = std::sin(angles_radians.y * 0.5f);
    const float cz = std::cos(angles_radians.z * 0.5f);
    const float sz = std::sin(angles_radians.z * 0.5f);

    return quat_normalize({
        cx * cy * cz + sx * sy * sz,
        sx * cy * cz - cx * sy * sz,
        cx * sy * cz + sx * cy * sz,
        cx * cy * sz - sx * sy * cz,
    });
}

inline quat quat_from_euler_zyx(const vec3& angles_radians) {
    const quat qx = quat_from_axis_angle({1.0f, 0.0f, 0.0f}, angles_radians.x);
    const quat qy = quat_from_axis_angle({0.0f, 1.0f, 0.0f}, angles_radians.y);
    const quat qz = quat_from_axis_angle({0.0f, 0.0f, 1.0f}, angles_radians.z);
    return quat_normalize(qz * qy * qx);
}

inline quat quat_from_euler(const vec3& angles_radians, euler_order order = EULER_ORDER_XYZ) {
    const quat qx = quat_from_axis_angle({1.0f, 0.0f, 0.0f}, angles_radians.x);
    const quat qy = quat_from_axis_angle({0.0f, 1.0f, 0.0f}, angles_radians.y);
    const quat qz = quat_from_axis_angle({0.0f, 0.0f, 1.0f}, angles_radians.z);

    switch (order) {
        case EULER_ORDER_XYZ:
            return quat_normalize(qx * qy * qz);
        case EULER_ORDER_XZY:
            return quat_normalize(qx * qz * qy);
        case EULER_ORDER_YXZ:
            return quat_normalize(qy * qx * qz);
        case EULER_ORDER_YZX:
            return quat_normalize(qy * qz * qx);
        case EULER_ORDER_ZXY:
            return quat_normalize(qz * qx * qy);
        case EULER_ORDER_ZYX:
            return quat_normalize(qz * qy * qx);
        default:
            return quat_from_euler_xyz(angles_radians);
    }
}

inline vec3 quat_to_euler_xyz(const quat& q) {
    const quat n = quat_normalize(q);

    const float sinr_cosp = 2.0f * (n.w * n.x + n.y * n.z);
    const float cosr_cosp = 1.0f - 2.0f * (n.x * n.x + n.y * n.y);
    const float roll = std::atan2(sinr_cosp, cosr_cosp);

    const float sinp = 2.0f * (n.w * n.y - n.z * n.x);
    float pitch = 0.0f;
    if (std::fabs(sinp) >= 1.0f) {
        pitch = std::copysign(1.57079632679f, sinp);
    } else {
        pitch = std::asin(sinp);
    }

    const float siny_cosp = 2.0f * (n.w * n.z + n.x * n.y);
    const float cosy_cosp = 1.0f - 2.0f * (n.y * n.y + n.z * n.z);
    const float yaw = std::atan2(siny_cosp, cosy_cosp);

    return {roll, pitch, yaw};
}
