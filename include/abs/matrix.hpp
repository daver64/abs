#pragma once

#include <cmath>

#include "abs/quaternion.hpp"
#include "abs/vec3.hpp"

struct mat3 {
    float m[3][3];
};

struct mat4 {
    float m[4][4];
};

inline mat3 mat3_identity() {
    return {{{1.0f, 0.0f, 0.0f},
             {0.0f, 1.0f, 0.0f},
             {0.0f, 0.0f, 1.0f}}};
}

inline mat4 mat4_identity() {
    return {{{1.0f, 0.0f, 0.0f, 0.0f},
             {0.0f, 1.0f, 0.0f, 0.0f},
             {0.0f, 0.0f, 1.0f, 0.0f},
             {0.0f, 0.0f, 0.0f, 1.0f}}};
}

inline mat3 mat3_mul(const mat3& a, const mat3& b) {
    mat3 out{};
    for (int r = 0; r < 3; ++r) {
        for (int c = 0; c < 3; ++c) {
            float sum = 0.0f;
            for (int k = 0; k < 3; ++k) {
                sum += a.m[r][k] * b.m[k][c];
            }
            out.m[r][c] = sum;
        }
    }
    return out;
}

inline mat4 mat4_mul(const mat4& a, const mat4& b) {
    mat4 out{};
    for (int r = 0; r < 4; ++r) {
        for (int c = 0; c < 4; ++c) {
            float sum = 0.0f;
            for (int k = 0; k < 4; ++k) {
                sum += a.m[r][k] * b.m[k][c];
            }
            out.m[r][c] = sum;
        }
    }
    return out;
}

inline vec3 mat3_mul_vec3(const mat3& m, const vec3& v) {
    return {
        m.m[0][0] * v.x + m.m[0][1] * v.y + m.m[0][2] * v.z,
        m.m[1][0] * v.x + m.m[1][1] * v.y + m.m[1][2] * v.z,
        m.m[2][0] * v.x + m.m[2][1] * v.y + m.m[2][2] * v.z,
    };
}

inline vec3 mat4_mul_vec3_point(const mat4& m, const vec3& v) {
    const float x = m.m[0][0] * v.x + m.m[0][1] * v.y + m.m[0][2] * v.z + m.m[0][3];
    const float y = m.m[1][0] * v.x + m.m[1][1] * v.y + m.m[1][2] * v.z + m.m[1][3];
    const float z = m.m[2][0] * v.x + m.m[2][1] * v.y + m.m[2][2] * v.z + m.m[2][3];
    const float w = m.m[3][0] * v.x + m.m[3][1] * v.y + m.m[3][2] * v.z + m.m[3][3];
    if (w == 0.0f) {
        return {x, y, z};
    }
    return {x / w, y / w, z / w};
}

inline vec3 mat4_mul_vec3_direction(const mat4& m, const vec3& v) {
    return {
        m.m[0][0] * v.x + m.m[0][1] * v.y + m.m[0][2] * v.z,
        m.m[1][0] * v.x + m.m[1][1] * v.y + m.m[1][2] * v.z,
        m.m[2][0] * v.x + m.m[2][1] * v.y + m.m[2][2] * v.z,
    };
}

inline mat4 mat4_translation(const vec3& t) {
    mat4 out = mat4_identity();
    out.m[0][3] = t.x;
    out.m[1][3] = t.y;
    out.m[2][3] = t.z;
    return out;
}

inline mat4 mat4_scale(const vec3& s) {
    return {{{s.x, 0.0f, 0.0f, 0.0f},
             {0.0f, s.y, 0.0f, 0.0f},
             {0.0f, 0.0f, s.z, 0.0f},
             {0.0f, 0.0f, 0.0f, 1.0f}}};
}

inline mat3 mat3_from_quat(const quat& q) {
    const quat n = quat_normalize(q);
    const float xx = n.x * n.x;
    const float yy = n.y * n.y;
    const float zz = n.z * n.z;
    const float xy = n.x * n.y;
    const float xz = n.x * n.z;
    const float yz = n.y * n.z;
    const float wx = n.w * n.x;
    const float wy = n.w * n.y;
    const float wz = n.w * n.z;

    return {{{1.0f - 2.0f * (yy + zz), 2.0f * (xy - wz), 2.0f * (xz + wy)},
             {2.0f * (xy + wz), 1.0f - 2.0f * (xx + zz), 2.0f * (yz - wx)},
             {2.0f * (xz - wy), 2.0f * (yz + wx), 1.0f - 2.0f * (xx + yy)}}};
}

inline mat4 mat4_from_quat(const quat& q) {
    const mat3 r = mat3_from_quat(q);
    return {{{r.m[0][0], r.m[0][1], r.m[0][2], 0.0f},
             {r.m[1][0], r.m[1][1], r.m[1][2], 0.0f},
             {r.m[2][0], r.m[2][1], r.m[2][2], 0.0f},
             {0.0f, 0.0f, 0.0f, 1.0f}}};
}

inline mat4 mat4_trs(const vec3& t, const quat& r, const vec3& s) {
    const mat4 mt = mat4_translation(t);
    const mat4 mr = mat4_from_quat(r);
    const mat4 ms = mat4_scale(s);
    return mat4_mul(mt, mat4_mul(mr, ms));
}
