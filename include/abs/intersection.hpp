#pragma once

#include <cmath>

#include "abs/geometry.hpp"

inline bool intersects_sphere_sphere(const sphere& a, const sphere& b) {
    const float r = a.radius + b.radius;
    return vec3_length_sq(a.center - b.center) <= r * r;
}

inline bool intersects_aabb_aabb(const aabb& a, const aabb& b) {
    if (a.max.x < b.min.x || a.min.x > b.max.x) return false;
    if (a.max.y < b.min.y || a.min.y > b.max.y) return false;
    if (a.max.z < b.min.z || a.min.z > b.max.z) return false;
    return true;
}

inline bool intersects_sphere_aabb(const sphere& s, const aabb& b) {
    float d2 = 0.0f;

    const float cx = s.center.x < b.min.x ? b.min.x : (s.center.x > b.max.x ? b.max.x : s.center.x);
    const float cy = s.center.y < b.min.y ? b.min.y : (s.center.y > b.max.y ? b.max.y : s.center.y);
    const float cz = s.center.z < b.min.z ? b.min.z : (s.center.z > b.max.z ? b.max.z : s.center.z);

    d2 += (s.center.x - cx) * (s.center.x - cx);
    d2 += (s.center.y - cy) * (s.center.y - cy);
    d2 += (s.center.z - cz) * (s.center.z - cz);
    return d2 <= s.radius * s.radius;
}

inline bool intersects_ray_plane(const ray& r, const plane& p, float& t_hit) {
    const float denom = vec3_dot(p.normal, r.direction);
    if (std::fabs(denom) <= 1e-8f) {
        return false;
    }
    t_hit = -(vec3_dot(p.normal, r.origin) + p.d) / denom;
    return t_hit >= 0.0f;
}

inline bool intersects_ray_sphere(const ray& r, const sphere& s, float& t_hit) {
    const vec3 oc = r.origin - s.center;
    const float a = vec3_dot(r.direction, r.direction);
    const float b = 2.0f * vec3_dot(oc, r.direction);
    const float c = vec3_dot(oc, oc) - s.radius * s.radius;
    const float disc = b * b - 4.0f * a * c;

    if (disc < 0.0f) {
        return false;
    }

    const float sq = std::sqrt(disc);
    const float t0 = (-b - sq) / (2.0f * a);
    const float t1 = (-b + sq) / (2.0f * a);

    if (t0 >= 0.0f) {
        t_hit = t0;
        return true;
    }
    if (t1 >= 0.0f) {
        t_hit = t1;
        return true;
    }
    return false;
}

inline bool intersects_ray_aabb(const ray& r, const aabb& b, float& t_min_out, float& t_max_out) {
    float tmin = 0.0f;
    float tmax = 1e30f;

    for (int axis = 0; axis < 3; ++axis) {
        const float ro = axis == 0 ? r.origin.x : (axis == 1 ? r.origin.y : r.origin.z);
        const float rd = axis == 0 ? r.direction.x : (axis == 1 ? r.direction.y : r.direction.z);
        const float bmin = axis == 0 ? b.min.x : (axis == 1 ? b.min.y : b.min.z);
        const float bmax = axis == 0 ? b.max.x : (axis == 1 ? b.max.y : b.max.z);

        if (std::fabs(rd) <= 1e-8f) {
            if (ro < bmin || ro > bmax) {
                return false;
            }
            continue;
        }

        const float inv_d = 1.0f / rd;
        float t0 = (bmin - ro) * inv_d;
        float t1 = (bmax - ro) * inv_d;
        if (t0 > t1) {
            const float tmp = t0;
            t0 = t1;
            t1 = tmp;
        }

        if (t0 > tmin) tmin = t0;
        if (t1 < tmax) tmax = t1;
        if (tmax < tmin) {
            return false;
        }
    }

    t_min_out = tmin;
    t_max_out = tmax;
    return true;
}

inline bool intersects_ray_triangle_moller(const ray& r,
                                           const triangle& tri,
                                           float& t_hit,
                                           float& u,
                                           float& v) {
    const vec3 e1 = tri.b - tri.a;
    const vec3 e2 = tri.c - tri.a;
    const vec3 pvec = vec3_cross(r.direction, e2);
    const float det = vec3_dot(e1, pvec);

    if (std::fabs(det) < 1e-8f) {
        return false;
    }

    const float inv_det = 1.0f / det;
    const vec3 tvec = r.origin - tri.a;
    u = vec3_dot(tvec, pvec) * inv_det;
    if (u < 0.0f || u > 1.0f) {
        return false;
    }

    const vec3 qvec = vec3_cross(tvec, e1);
    v = vec3_dot(r.direction, qvec) * inv_det;
    if (v < 0.0f || (u + v) > 1.0f) {
        return false;
    }

    t_hit = vec3_dot(e2, qvec) * inv_det;
    return t_hit >= 0.0f;
}

inline bool intersects_frustum_sphere(const frustum& f, const sphere& s) {
    for (int i = 0; i < 6; ++i) {
        if (plane_signed_distance(f.planes[i], s.center) < -s.radius) {
            return false;
        }
    }
    return true;
}

inline bool intersects_frustum_aabb(const frustum& f, const aabb& b) {
    for (int i = 0; i < 6; ++i) {
        const plane& p = f.planes[i];
        const vec3 v = {
            p.normal.x >= 0.0f ? b.max.x : b.min.x,
            p.normal.y >= 0.0f ? b.max.y : b.min.y,
            p.normal.z >= 0.0f ? b.max.z : b.min.z,
        };
        if (plane_signed_distance(p, v) < 0.0f) {
            return false;
        }
    }
    return true;
}
