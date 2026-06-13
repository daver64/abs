#pragma once

#include <cmath>

#include "abs/matrix.hpp"
#include "abs/vec3.hpp"

struct ray {
    vec3 origin;
    vec3 direction;
};

struct plane {
    vec3 normal;
    float d;
};

struct sphere {
    vec3 center;
    float radius;
};

struct aabb {
    vec3 min;
    vec3 max;
};

struct obb {
    vec3 center;
    vec3 axis[3];
    vec3 extent;
};

struct triangle {
    vec3 a;
    vec3 b;
    vec3 c;
};

struct frustum {
    plane planes[6];
};

inline plane plane_from_point_normal(const vec3& point, const vec3& normal) {
    const vec3 n = vec3_normalize(normal);
    return {n, -vec3_dot(n, point)};
}

inline float plane_signed_distance(const plane& p, const vec3& point) {
    return vec3_dot(p.normal, point) + p.d;
}

inline vec3 triangle_normal(const triangle& t) {
    return vec3_normalize(vec3_cross(t.b - t.a, t.c - t.a));
}

inline float triangle_area(const triangle& t) {
    return 0.5f * vec3_length(vec3_cross(t.b - t.a, t.c - t.a));
}

inline vec3 triangle_barycentric(const triangle& t, const vec3& p) {
    const vec3 v0 = t.b - t.a;
    const vec3 v1 = t.c - t.a;
    const vec3 v2 = p - t.a;
    const float d00 = vec3_dot(v0, v0);
    const float d01 = vec3_dot(v0, v1);
    const float d11 = vec3_dot(v1, v1);
    const float d20 = vec3_dot(v2, v0);
    const float d21 = vec3_dot(v2, v1);
    const float denom = d00 * d11 - d01 * d01;
    if (std::fabs(denom) <= 1e-8f) {
        return {1.0f, 0.0f, 0.0f};
    }
    const float v = (d11 * d20 - d01 * d21) / denom;
    const float w = (d00 * d21 - d01 * d20) / denom;
    const float u = 1.0f - v - w;
    return {u, v, w};
}

inline plane plane_normalize(const plane& p) {
    const float len = vec3_length(p.normal);
    if (len <= 0.0f) {
        return {{0.0f, 0.0f, 1.0f}, 0.0f};
    }
    return {p.normal / len, p.d / len};
}

inline frustum frustum_from_view_projection(const mat4& m) {
    frustum f{};

    f.planes[0] = plane_normalize({
        {m.m[0][3] + m.m[0][0], m.m[1][3] + m.m[1][0], m.m[2][3] + m.m[2][0]},
        m.m[3][3] + m.m[3][0],
    });
    f.planes[1] = plane_normalize({
        {m.m[0][3] - m.m[0][0], m.m[1][3] - m.m[1][0], m.m[2][3] - m.m[2][0]},
        m.m[3][3] - m.m[3][0],
    });
    f.planes[2] = plane_normalize({
        {m.m[0][3] + m.m[0][1], m.m[1][3] + m.m[1][1], m.m[2][3] + m.m[2][1]},
        m.m[3][3] + m.m[3][1],
    });
    f.planes[3] = plane_normalize({
        {m.m[0][3] - m.m[0][1], m.m[1][3] - m.m[1][1], m.m[2][3] - m.m[2][1]},
        m.m[3][3] - m.m[3][1],
    });
    f.planes[4] = plane_normalize({
        {m.m[0][3] + m.m[0][2], m.m[1][3] + m.m[1][2], m.m[2][3] + m.m[2][2]},
        m.m[3][3] + m.m[3][2],
    });
    f.planes[5] = plane_normalize({
        {m.m[0][3] - m.m[0][2], m.m[1][3] - m.m[1][2], m.m[2][3] - m.m[2][2]},
        m.m[3][3] - m.m[3][2],
    });

    return f;
}
