#pragma once

#include <cmath>

#include "abs/vec3.hpp"

struct polar_coord {
    float radius;
    float angle;
};

struct spherical_coord {
    float radius;
    float azimuth;
    float elevation;
};

inline polar_coord polar_from_vec3_xy(const vec3& v) {
    return {
        std::sqrt(v.x * v.x + v.y * v.y),
        std::atan2(v.y, v.x),
    };
}

inline vec3 polar_to_vec3_xy(const polar_coord& p, float z = 0.0f) {
    return {
        p.radius * std::cos(p.angle),
        p.radius * std::sin(p.angle),
        z,
    };
}

inline spherical_coord spherical_from_vec3(const vec3& v) {
    const float radius = std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
    const float planar = std::sqrt(v.x * v.x + v.y * v.y);
    if (radius <= 0.0f) {
        return {0.0f, 0.0f, 0.0f};
    }
    return {
        radius,
        std::atan2(v.y, v.x),
        std::atan2(v.z, planar),
    };
}

inline vec3 spherical_to_vec3(const spherical_coord& s) {
    const float planar = s.radius * std::cos(s.elevation);
    return {
        planar * std::cos(s.azimuth),
        planar * std::sin(s.azimuth),
        s.radius * std::sin(s.elevation),
    };
}
