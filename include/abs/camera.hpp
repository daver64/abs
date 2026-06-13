#pragma once

#include <cmath>

#include "abs/matrix.hpp"
#include "abs/vec3.hpp"

enum handedness {
    HANDEDNESS_RIGHT,
    HANDEDNESS_LEFT,
};

inline mat4 mat4_look_at(const vec3& eye, const vec3& target, const vec3& up, handedness h = HANDEDNESS_RIGHT) {
    if (h == HANDEDNESS_RIGHT) {
        const vec3 f = vec3_normalize(target - eye);
        const vec3 s = vec3_normalize(vec3_cross(f, up));
        const vec3 u = vec3_cross(s, f);

        return {{{s.x, s.y, s.z, -vec3_dot(s, eye)},
                 {u.x, u.y, u.z, -vec3_dot(u, eye)},
                 {-f.x, -f.y, -f.z, vec3_dot(f, eye)},
                 {0.0f, 0.0f, 0.0f, 1.0f}}};
    }

    const vec3 f = vec3_normalize(target - eye);
    const vec3 s = vec3_normalize(vec3_cross(up, f));
    const vec3 u = vec3_cross(f, s);

    return {{{s.x, s.y, s.z, -vec3_dot(s, eye)},
             {u.x, u.y, u.z, -vec3_dot(u, eye)},
             {f.x, f.y, f.z, -vec3_dot(f, eye)},
             {0.0f, 0.0f, 0.0f, 1.0f}}};
}

inline mat4 mat4_perspective(float fov_y_radians,
                             float aspect,
                             float z_near,
                             float z_far,
                             handedness h = HANDEDNESS_RIGHT) {
    const float f = 1.0f / std::tan(fov_y_radians * 0.5f);
    const float range_inv = 1.0f / (z_near - z_far);

    if (h == HANDEDNESS_RIGHT) {
        return {{{f / aspect, 0.0f, 0.0f, 0.0f},
                 {0.0f, f, 0.0f, 0.0f},
                 {0.0f, 0.0f, (z_far + z_near) * range_inv, 2.0f * z_far * z_near * range_inv},
                 {0.0f, 0.0f, -1.0f, 0.0f}}};
    }

    return {{{f / aspect, 0.0f, 0.0f, 0.0f},
             {0.0f, f, 0.0f, 0.0f},
             {0.0f, 0.0f, z_far / (z_far - z_near), (-z_near * z_far) / (z_far - z_near)},
             {0.0f, 0.0f, 1.0f, 0.0f}}};
}

inline mat4 mat4_ortho(float left,
                       float right,
                       float bottom,
                       float top,
                       float z_near,
                       float z_far,
                       handedness h = HANDEDNESS_RIGHT) {
    const float rl = right - left;
    const float tb = top - bottom;
    const float fn = z_far - z_near;

    if (h == HANDEDNESS_RIGHT) {
        return {{{2.0f / rl, 0.0f, 0.0f, -(right + left) / rl},
                 {0.0f, 2.0f / tb, 0.0f, -(top + bottom) / tb},
                 {0.0f, 0.0f, -2.0f / fn, -(z_far + z_near) / fn},
                 {0.0f, 0.0f, 0.0f, 1.0f}}};
    }

    return {{{2.0f / rl, 0.0f, 0.0f, -(right + left) / rl},
             {0.0f, 2.0f / tb, 0.0f, -(top + bottom) / tb},
             {0.0f, 0.0f, 1.0f / fn, -z_near / fn},
             {0.0f, 0.0f, 0.0f, 1.0f}}};
}
