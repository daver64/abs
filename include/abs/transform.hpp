#pragma once

#include <cmath>

#include "abs/matrix.hpp"
#include "abs/quaternion.hpp"
#include "abs/vec3.hpp"

inline vec3 world_to_local_point(const mat4& world_from_local, const vec3& p_world) {
    return mat4_mul_vec3_point(mat4_inverse_affine(world_from_local), p_world);
}

inline vec3 local_to_world_point(const mat4& world_from_local, const vec3& p_local) {
    return mat4_mul_vec3_point(world_from_local, p_local);
}

inline vec3 world_to_local_direction(const mat4& world_from_local, const vec3& d_world) {
    return mat4_mul_vec3_direction(mat4_inverse_affine(world_from_local), d_world);
}

inline vec3 local_to_world_direction(const mat4& world_from_local, const vec3& d_local) {
    return mat4_mul_vec3_direction(world_from_local, d_local);
}

inline vec3 handedness_flip_z_vec3(const vec3& v) {
    return {v.x, v.y, -v.z};
}

inline quat handedness_flip_z_quat(const quat& q) {
    return {q.w, -q.x, -q.y, q.z};
}

inline mat4 handedness_flip_z_mat4(const mat4& m) {
    mat4 out = m;
    out.m[0][2] = -out.m[0][2];
    out.m[1][2] = -out.m[1][2];
    out.m[2][0] = -out.m[2][0];
    out.m[2][1] = -out.m[2][1];
    out.m[2][3] = -out.m[2][3];
    out.m[3][2] = -out.m[3][2];
    return out;
}

inline void build_tbn(const vec3& normal,
                      const vec3& tangent_hint,
                      vec3& out_tangent,
                      vec3& out_bitangent,
                      vec3& out_normal) {
    out_normal = vec3_normalize(normal);
    vec3 t = tangent_hint - out_normal * vec3_dot(tangent_hint, out_normal);
    if (vec3_length_sq(t) <= 1e-8f) {
        t = std::fabs(out_normal.x) < 0.9f ? vec3_cross({1.0f, 0.0f, 0.0f}, out_normal)
                                           : vec3_cross({0.0f, 1.0f, 0.0f}, out_normal);
    }
    out_tangent = vec3_normalize(t);
    out_bitangent = vec3_normalize(vec3_cross(out_normal, out_tangent));
}
