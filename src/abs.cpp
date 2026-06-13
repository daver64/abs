#include <cstdio>

#include "abs/abs.hpp"

void print_vec3(const vec3& v) {
    printf("vec3: (%f, %f, %f)\n", v.x, v.y, v.z);
}

void print_quat(const quat& q) {
    printf("quat: (%f, %f, %f, %f)\n", q.w, q.x, q.y, q.z);
}

int main() {
    printf("SIMD backend: %s\n", abs_simd_backend_name());

    vec3 a{1.0f, 2.0f, 3.0f};
    vec3 b{4.0f, 5.0f, 6.0f};

    vec3 c = a + b;
    vec3 d = a - b;
    vec3 e = a * 2.0f;
    vec3 f = a / 2.0f;

    print_vec3(c);
    print_vec3(d);
    print_vec3(e);
    print_vec3(f);

    printf("dot(a, b): %f\n", vec3_dot(a, b));
    print_vec3(vec3_cross(a, b));

    quat q = quat_from_axis_angle({0.0f, 1.0f, 0.0f}, 1.57079632679f);
    print_quat(q);
    print_vec3(quat_rotate_vec3(q, {1.0f, 0.0f, 0.0f}));

    mat4 trs = mat4_trs({1.0f, 2.0f, 3.0f}, q, {2.0f, 2.0f, 2.0f});
    print_vec3(mat4_mul_vec3_point(trs, {1.0f, 0.0f, 0.0f}));

    spherical_coord sph = spherical_from_vec3({1.0f, 1.0f, 1.0f});
    printf("spherical: (r=%f, az=%f, el=%f)\n", sph.radius, sph.azimuth, sph.elevation);
    print_vec3(spherical_to_vec3(sph));

    vec4f p = vec4f_from_vec3(a, 1.0f);
    vec4f qv = vec4f_add(p, vec4f_set(1.0f, 1.0f, 1.0f, 0.0f));
    printf("vec4f dot4: %f\n", vec4f_dot4(p, qv));

    return 0;
}