#pragma once

#include <cmath>

#include "abs/vec3.hpp"

inline float lerpf(float a, float b, float t) {
    return a + (b - a) * t;
}

inline float smoothstepf(float edge0, float edge1, float x) {
    const float t = (x - edge0) / (edge1 - edge0);
    const float s = t < 0.0f ? 0.0f : (t > 1.0f ? 1.0f : t);
    return s * s * (3.0f - 2.0f * s);
}

inline float ease_in_quad(float t) {
    return t * t;
}

inline float ease_out_quad(float t) {
    return t * (2.0f - t);
}

inline float ease_in_out_cubic(float t) {
    if (t < 0.5f) {
        return 4.0f * t * t * t;
    }
    const float f = -2.0f * t + 2.0f;
    return 1.0f - (f * f * f) * 0.5f;
}

inline float hermitef(float p0, float m0, float p1, float m1, float t) {
    const float t2 = t * t;
    const float t3 = t2 * t;
    return (2.0f * t3 - 3.0f * t2 + 1.0f) * p0
         + (t3 - 2.0f * t2 + t) * m0
         + (-2.0f * t3 + 3.0f * t2) * p1
         + (t3 - t2) * m1;
}

inline vec3 hermite_vec3(const vec3& p0, const vec3& m0, const vec3& p1, const vec3& m1, float t) {
    return {
        hermitef(p0.x, m0.x, p1.x, m1.x, t),
        hermitef(p0.y, m0.y, p1.y, m1.y, t),
        hermitef(p0.z, m0.z, p1.z, m1.z, t),
    };
}

inline vec3 catmull_rom_vec3(const vec3& p0, const vec3& p1, const vec3& p2, const vec3& p3, float t) {
    const vec3 m1 = (p2 - p0) * 0.5f;
    const vec3 m2 = (p3 - p1) * 0.5f;
    return hermite_vec3(p1, m1, p2, m2, t);
}

inline vec3 bezier_cubic_vec3(const vec3& p0,
                              const vec3& p1,
                              const vec3& p2,
                              const vec3& p3,
                              float t) {
    const float u = 1.0f - t;
    const float uu = u * u;
    const float tt = t * t;
    return p0 * (uu * u)
         + p1 * (3.0f * uu * t)
         + p2 * (3.0f * u * tt)
         + p3 * (tt * t);
}

inline float smooth_damp(float current,
                         float target,
                         float& current_velocity,
                         float smooth_time,
                         float delta_time,
                         float max_speed = 1e30f) {
    smooth_time = smooth_time < 0.0001f ? 0.0001f : smooth_time;
    const float omega = 2.0f / smooth_time;
    const float x = omega * delta_time;
    const float decay = 1.0f / (1.0f + x + 0.48f * x * x + 0.235f * x * x * x);

    float change = current - target;
    const float original_to = target;

    const float max_change = max_speed * smooth_time;
    if (change > max_change) {
        change = max_change;
    } else if (change < -max_change) {
        change = -max_change;
    }

    target = current - change;
    const float temp = (current_velocity + omega * change) * delta_time;
    current_velocity = (current_velocity - omega * temp) * decay;
    float output = target + (change + temp) * decay;

    if ((original_to - current > 0.0f) == (output > original_to)) {
        output = original_to;
        current_velocity = (output - original_to) / delta_time;
    }

    return output;
}

inline vec3 smooth_damp_vec3(vec3 current,
                             vec3 target,
                             vec3& current_velocity,
                             float smooth_time,
                             float delta_time,
                             float max_speed = 1e30f) {
    current.x = smooth_damp(current.x, target.x, current_velocity.x, smooth_time, delta_time, max_speed);
    current.y = smooth_damp(current.y, target.y, current_velocity.y, smooth_time, delta_time, max_speed);
    current.z = smooth_damp(current.z, target.z, current_velocity.z, smooth_time, delta_time, max_speed);
    return current;
}

inline float spring_critical(float current,
                             float target,
                             float& velocity,
                             float half_life,
                             float delta_time) {
    const float y = 0.69314718056f / (half_life > 0.0001f ? half_life : 0.0001f);
    const float j0 = current - target;
    const float j1 = velocity + j0 * y;
    const float e = std::exp(-y * delta_time);
    current = e * (j0 + j1 * delta_time) + target;
    velocity = e * (velocity - j1 * y * delta_time);
    return current;
 }
