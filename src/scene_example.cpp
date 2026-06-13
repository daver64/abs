#include <cstdio>

#include "abs/abs.hpp"

static void print_vec3(const char* label, const vec3& v) {
    std::printf("%s: (%f, %f, %f)\n", label, v.x, v.y, v.z);
}

int main() {
    std::printf("abs scene example\n");

    const vec3 camera_pos{0.0f, 1.5f, 6.0f};
    const mat4 view = mat4_look_at(camera_pos, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f, 0.0f});
    const mat4 proj = mat4_perspective(1.0471975512f, 16.0f / 9.0f, 0.1f, 100.0f);
    const frustum fr = frustum_from_view_projection(mat4_mul(proj, view));

    const sphere scene_spheres[] = {
        {{0.0f, 1.0f, 0.0f}, 1.0f},
        {{3.0f, 1.0f, -4.0f}, 0.8f},
        {{-8.0f, 1.0f, -2.0f}, 1.5f},
    };

    for (int i = 0; i < 3; ++i) {
        const bool visible = intersects_frustum_sphere(fr, scene_spheres[i]);
        std::printf("sphere[%d] visible: %s\n", i, visible ? "yes" : "no");
    }

    ray pick_ray{};
    pick_ray.origin = camera_pos;
    pick_ray.direction = vec3_normalize(vec3{0.0f, -0.2f, -1.0f});

    float nearest_t = 1e30f;
    int hit_index = -1;
    for (int i = 0; i < 3; ++i) {
        float t_hit = 0.0f;
        if (intersects_ray_sphere(pick_ray, scene_spheres[i], t_hit) && t_hit < nearest_t) {
            nearest_t = t_hit;
            hit_index = i;
        }
    }

    if (hit_index >= 0) {
        const vec3 hit_point = pick_ray.origin + pick_ray.direction * nearest_t;
        std::printf("pick hit sphere[%d] at t=%f\n", hit_index, nearest_t);
        print_vec3("hit point", hit_point);
    } else {
        std::printf("pick missed all spheres\n");
    }

    const triangle ground_tri{{-2.0f, 0.0f, -2.0f}, {2.0f, 0.0f, -2.0f}, {0.0f, 0.0f, 2.0f}};
    float t_tri = 0.0f;
    float u = 0.0f;
    float v = 0.0f;
    if (intersects_ray_triangle_moller(pick_ray, ground_tri, t_tri, u, v)) {
        const vec3 p = pick_ray.origin + pick_ray.direction * t_tri;
        const vec3 bary = triangle_barycentric(ground_tri, p);
        std::printf("ray hit ground triangle at t=%f\n", t_tri);
        print_vec3("barycentric", bary);
    } else {
        std::printf("ray missed ground triangle\n");
    }

    vec3 tangent{};
    vec3 bitangent{};
    vec3 normal{};
    build_tbn({0.0f, 1.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, tangent, bitangent, normal);
    print_vec3("tangent", tangent);
    print_vec3("bitangent", bitangent);
    print_vec3("normal", normal);

    float yaw = 7.5f;
    yaw = wrap_angle_pi(yaw);
    std::printf("wrapped yaw: %f\n", yaw);

    vec3 h_sample = sample_cosine_hemisphere(0.42f, 0.73f);
    print_vec3("cosine hemisphere sample", h_sample);

    return 0;
}
