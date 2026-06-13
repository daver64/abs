#pragma once

#include <cstddef>

#include "abs/matrix.hpp"
#include "abs/simd.hpp"
#include "abs/vec3.hpp"

struct vec3_soa_const_view {
    const float* x;
    const float* y;
    const float* z;
};

struct vec3_soa_view {
    float* x;
    float* y;
    float* z;
};

inline void vec3_dot_batch_soa(const vec3_soa_const_view& a,
                               const vec3_soa_const_view& b,
                               float* out,
                               std::size_t count) {
    std::size_t i = 0;

#if ABS_SIMD_BACKEND == ABS_SIMD_AVX2
    for (; i + 8 <= count; i += 8) {
        const __m256 ax = _mm256_loadu_ps(a.x + i);
        const __m256 ay = _mm256_loadu_ps(a.y + i);
        const __m256 az = _mm256_loadu_ps(a.z + i);
        const __m256 bx = _mm256_loadu_ps(b.x + i);
        const __m256 by = _mm256_loadu_ps(b.y + i);
        const __m256 bz = _mm256_loadu_ps(b.z + i);

        __m256 sum = _mm256_mul_ps(ax, bx);
        sum = _mm256_add_ps(sum, _mm256_mul_ps(ay, by));
        sum = _mm256_add_ps(sum, _mm256_mul_ps(az, bz));
        _mm256_storeu_ps(out + i, sum);
    }
#endif

#if ABS_SIMD_BACKEND == ABS_SIMD_SSE2
    for (; i + 4 <= count; i += 4) {
        const __m128 ax = _mm_loadu_ps(a.x + i);
        const __m128 ay = _mm_loadu_ps(a.y + i);
        const __m128 az = _mm_loadu_ps(a.z + i);
        const __m128 bx = _mm_loadu_ps(b.x + i);
        const __m128 by = _mm_loadu_ps(b.y + i);
        const __m128 bz = _mm_loadu_ps(b.z + i);

        __m128 sum = _mm_mul_ps(ax, bx);
        sum = _mm_add_ps(sum, _mm_mul_ps(ay, by));
        sum = _mm_add_ps(sum, _mm_mul_ps(az, bz));
        _mm_storeu_ps(out + i, sum);
    }
#endif

#if ABS_SIMD_BACKEND == ABS_SIMD_NEON
    for (; i + 4 <= count; i += 4) {
        const float32x4_t ax = vld1q_f32(a.x + i);
        const float32x4_t ay = vld1q_f32(a.y + i);
        const float32x4_t az = vld1q_f32(a.z + i);
        const float32x4_t bx = vld1q_f32(b.x + i);
        const float32x4_t by = vld1q_f32(b.y + i);
        const float32x4_t bz = vld1q_f32(b.z + i);

        float32x4_t sum = vmulq_f32(ax, bx);
        sum = vmlaq_f32(sum, ay, by);
        sum = vmlaq_f32(sum, az, bz);
        vst1q_f32(out + i, sum);
    }
#endif

    for (; i < count; ++i) {
        out[i] = a.x[i] * b.x[i] + a.y[i] * b.y[i] + a.z[i] * b.z[i];
    }
}

inline void vec3_cross_batch_soa(const vec3_soa_const_view& a,
                                 const vec3_soa_const_view& b,
                                 vec3_soa_view out,
                                 std::size_t count) {
    std::size_t i = 0;

#if ABS_SIMD_BACKEND == ABS_SIMD_AVX2
    for (; i + 8 <= count; i += 8) {
        const __m256 ax = _mm256_loadu_ps(a.x + i);
        const __m256 ay = _mm256_loadu_ps(a.y + i);
        const __m256 az = _mm256_loadu_ps(a.z + i);
        const __m256 bx = _mm256_loadu_ps(b.x + i);
        const __m256 by = _mm256_loadu_ps(b.y + i);
        const __m256 bz = _mm256_loadu_ps(b.z + i);

        const __m256 cx = _mm256_sub_ps(_mm256_mul_ps(ay, bz), _mm256_mul_ps(az, by));
        const __m256 cy = _mm256_sub_ps(_mm256_mul_ps(az, bx), _mm256_mul_ps(ax, bz));
        const __m256 cz = _mm256_sub_ps(_mm256_mul_ps(ax, by), _mm256_mul_ps(ay, bx));

        _mm256_storeu_ps(out.x + i, cx);
        _mm256_storeu_ps(out.y + i, cy);
        _mm256_storeu_ps(out.z + i, cz);
    }
#endif

#if ABS_SIMD_BACKEND == ABS_SIMD_SSE2
    for (; i + 4 <= count; i += 4) {
        const __m128 ax = _mm_loadu_ps(a.x + i);
        const __m128 ay = _mm_loadu_ps(a.y + i);
        const __m128 az = _mm_loadu_ps(a.z + i);
        const __m128 bx = _mm_loadu_ps(b.x + i);
        const __m128 by = _mm_loadu_ps(b.y + i);
        const __m128 bz = _mm_loadu_ps(b.z + i);

        const __m128 cx = _mm_sub_ps(_mm_mul_ps(ay, bz), _mm_mul_ps(az, by));
        const __m128 cy = _mm_sub_ps(_mm_mul_ps(az, bx), _mm_mul_ps(ax, bz));
        const __m128 cz = _mm_sub_ps(_mm_mul_ps(ax, by), _mm_mul_ps(ay, bx));

        _mm_storeu_ps(out.x + i, cx);
        _mm_storeu_ps(out.y + i, cy);
        _mm_storeu_ps(out.z + i, cz);
    }
#endif

#if ABS_SIMD_BACKEND == ABS_SIMD_NEON
    for (; i + 4 <= count; i += 4) {
        const float32x4_t ax = vld1q_f32(a.x + i);
        const float32x4_t ay = vld1q_f32(a.y + i);
        const float32x4_t az = vld1q_f32(a.z + i);
        const float32x4_t bx = vld1q_f32(b.x + i);
        const float32x4_t by = vld1q_f32(b.y + i);
        const float32x4_t bz = vld1q_f32(b.z + i);

        const float32x4_t cx = vsubq_f32(vmulq_f32(ay, bz), vmulq_f32(az, by));
        const float32x4_t cy = vsubq_f32(vmulq_f32(az, bx), vmulq_f32(ax, bz));
        const float32x4_t cz = vsubq_f32(vmulq_f32(ax, by), vmulq_f32(ay, bx));

        vst1q_f32(out.x + i, cx);
        vst1q_f32(out.y + i, cy);
        vst1q_f32(out.z + i, cz);
    }
#endif

    for (; i < count; ++i) {
        out.x[i] = a.y[i] * b.z[i] - a.z[i] * b.y[i];
        out.y[i] = a.z[i] * b.x[i] - a.x[i] * b.z[i];
        out.z[i] = a.x[i] * b.y[i] - a.y[i] * b.x[i];
    }
}

inline void mat4_transform_points_soa_affine(const mat4& m,
                                             const vec3_soa_const_view& in,
                                             vec3_soa_view out,
                                             std::size_t count) {
    std::size_t i = 0;

#if ABS_SIMD_BACKEND == ABS_SIMD_AVX2
    const __m256 m00 = _mm256_set1_ps(m.m[0][0]);
    const __m256 m01 = _mm256_set1_ps(m.m[0][1]);
    const __m256 m02 = _mm256_set1_ps(m.m[0][2]);
    const __m256 m03 = _mm256_set1_ps(m.m[0][3]);

    const __m256 m10 = _mm256_set1_ps(m.m[1][0]);
    const __m256 m11 = _mm256_set1_ps(m.m[1][1]);
    const __m256 m12 = _mm256_set1_ps(m.m[1][2]);
    const __m256 m13 = _mm256_set1_ps(m.m[1][3]);

    const __m256 m20 = _mm256_set1_ps(m.m[2][0]);
    const __m256 m21 = _mm256_set1_ps(m.m[2][1]);
    const __m256 m22 = _mm256_set1_ps(m.m[2][2]);
    const __m256 m23 = _mm256_set1_ps(m.m[2][3]);

    for (; i + 8 <= count; i += 8) {
        const __m256 x = _mm256_loadu_ps(in.x + i);
        const __m256 y = _mm256_loadu_ps(in.y + i);
        const __m256 z = _mm256_loadu_ps(in.z + i);

        __m256 ox = _mm256_mul_ps(x, m00);
        ox = _mm256_add_ps(ox, _mm256_mul_ps(y, m01));
        ox = _mm256_add_ps(ox, _mm256_mul_ps(z, m02));
        ox = _mm256_add_ps(ox, m03);

        __m256 oy = _mm256_mul_ps(x, m10);
        oy = _mm256_add_ps(oy, _mm256_mul_ps(y, m11));
        oy = _mm256_add_ps(oy, _mm256_mul_ps(z, m12));
        oy = _mm256_add_ps(oy, m13);

        __m256 oz = _mm256_mul_ps(x, m20);
        oz = _mm256_add_ps(oz, _mm256_mul_ps(y, m21));
        oz = _mm256_add_ps(oz, _mm256_mul_ps(z, m22));
        oz = _mm256_add_ps(oz, m23);

        _mm256_storeu_ps(out.x + i, ox);
        _mm256_storeu_ps(out.y + i, oy);
        _mm256_storeu_ps(out.z + i, oz);
    }
#endif

#if ABS_SIMD_BACKEND == ABS_SIMD_SSE2
    const __m128 m00 = _mm_set1_ps(m.m[0][0]);
    const __m128 m01 = _mm_set1_ps(m.m[0][1]);
    const __m128 m02 = _mm_set1_ps(m.m[0][2]);
    const __m128 m03 = _mm_set1_ps(m.m[0][3]);

    const __m128 m10 = _mm_set1_ps(m.m[1][0]);
    const __m128 m11 = _mm_set1_ps(m.m[1][1]);
    const __m128 m12 = _mm_set1_ps(m.m[1][2]);
    const __m128 m13 = _mm_set1_ps(m.m[1][3]);

    const __m128 m20 = _mm_set1_ps(m.m[2][0]);
    const __m128 m21 = _mm_set1_ps(m.m[2][1]);
    const __m128 m22 = _mm_set1_ps(m.m[2][2]);
    const __m128 m23 = _mm_set1_ps(m.m[2][3]);

    for (; i + 4 <= count; i += 4) {
        const __m128 x = _mm_loadu_ps(in.x + i);
        const __m128 y = _mm_loadu_ps(in.y + i);
        const __m128 z = _mm_loadu_ps(in.z + i);

        __m128 ox = _mm_mul_ps(x, m00);
        ox = _mm_add_ps(ox, _mm_mul_ps(y, m01));
        ox = _mm_add_ps(ox, _mm_mul_ps(z, m02));
        ox = _mm_add_ps(ox, m03);

        __m128 oy = _mm_mul_ps(x, m10);
        oy = _mm_add_ps(oy, _mm_mul_ps(y, m11));
        oy = _mm_add_ps(oy, _mm_mul_ps(z, m12));
        oy = _mm_add_ps(oy, m13);

        __m128 oz = _mm_mul_ps(x, m20);
        oz = _mm_add_ps(oz, _mm_mul_ps(y, m21));
        oz = _mm_add_ps(oz, _mm_mul_ps(z, m22));
        oz = _mm_add_ps(oz, m23);

        _mm_storeu_ps(out.x + i, ox);
        _mm_storeu_ps(out.y + i, oy);
        _mm_storeu_ps(out.z + i, oz);
    }
#endif

#if ABS_SIMD_BACKEND == ABS_SIMD_NEON
    const float32x4_t m00 = vdupq_n_f32(m.m[0][0]);
    const float32x4_t m01 = vdupq_n_f32(m.m[0][1]);
    const float32x4_t m02 = vdupq_n_f32(m.m[0][2]);
    const float32x4_t m03 = vdupq_n_f32(m.m[0][3]);

    const float32x4_t m10 = vdupq_n_f32(m.m[1][0]);
    const float32x4_t m11 = vdupq_n_f32(m.m[1][1]);
    const float32x4_t m12 = vdupq_n_f32(m.m[1][2]);
    const float32x4_t m13 = vdupq_n_f32(m.m[1][3]);

    const float32x4_t m20 = vdupq_n_f32(m.m[2][0]);
    const float32x4_t m21 = vdupq_n_f32(m.m[2][1]);
    const float32x4_t m22 = vdupq_n_f32(m.m[2][2]);
    const float32x4_t m23 = vdupq_n_f32(m.m[2][3]);

    for (; i + 4 <= count; i += 4) {
        const float32x4_t x = vld1q_f32(in.x + i);
        const float32x4_t y = vld1q_f32(in.y + i);
        const float32x4_t z = vld1q_f32(in.z + i);

        float32x4_t ox = vmulq_f32(x, m00);
        ox = vmlaq_f32(ox, y, m01);
        ox = vmlaq_f32(ox, z, m02);
        ox = vaddq_f32(ox, m03);

        float32x4_t oy = vmulq_f32(x, m10);
        oy = vmlaq_f32(oy, y, m11);
        oy = vmlaq_f32(oy, z, m12);
        oy = vaddq_f32(oy, m13);

        float32x4_t oz = vmulq_f32(x, m20);
        oz = vmlaq_f32(oz, y, m21);
        oz = vmlaq_f32(oz, z, m22);
        oz = vaddq_f32(oz, m23);

        vst1q_f32(out.x + i, ox);
        vst1q_f32(out.y + i, oy);
        vst1q_f32(out.z + i, oz);
    }
#endif

    for (; i < count; ++i) {
        out.x[i] = m.m[0][0] * in.x[i] + m.m[0][1] * in.y[i] + m.m[0][2] * in.z[i] + m.m[0][3];
        out.y[i] = m.m[1][0] * in.x[i] + m.m[1][1] * in.y[i] + m.m[1][2] * in.z[i] + m.m[1][3];
        out.z[i] = m.m[2][0] * in.x[i] + m.m[2][1] * in.y[i] + m.m[2][2] * in.z[i] + m.m[2][3];
    }
}

inline void mat4_transform_directions_soa(const mat4& m,
                                          const vec3_soa_const_view& in,
                                          vec3_soa_view out,
                                          std::size_t count) {
    std::size_t i = 0;

#if ABS_SIMD_BACKEND == ABS_SIMD_AVX2
    const __m256 m00 = _mm256_set1_ps(m.m[0][0]);
    const __m256 m01 = _mm256_set1_ps(m.m[0][1]);
    const __m256 m02 = _mm256_set1_ps(m.m[0][2]);

    const __m256 m10 = _mm256_set1_ps(m.m[1][0]);
    const __m256 m11 = _mm256_set1_ps(m.m[1][1]);
    const __m256 m12 = _mm256_set1_ps(m.m[1][2]);

    const __m256 m20 = _mm256_set1_ps(m.m[2][0]);
    const __m256 m21 = _mm256_set1_ps(m.m[2][1]);
    const __m256 m22 = _mm256_set1_ps(m.m[2][2]);

    for (; i + 8 <= count; i += 8) {
        const __m256 x = _mm256_loadu_ps(in.x + i);
        const __m256 y = _mm256_loadu_ps(in.y + i);
        const __m256 z = _mm256_loadu_ps(in.z + i);

        __m256 ox = _mm256_mul_ps(x, m00);
        ox = _mm256_add_ps(ox, _mm256_mul_ps(y, m01));
        ox = _mm256_add_ps(ox, _mm256_mul_ps(z, m02));

        __m256 oy = _mm256_mul_ps(x, m10);
        oy = _mm256_add_ps(oy, _mm256_mul_ps(y, m11));
        oy = _mm256_add_ps(oy, _mm256_mul_ps(z, m12));

        __m256 oz = _mm256_mul_ps(x, m20);
        oz = _mm256_add_ps(oz, _mm256_mul_ps(y, m21));
        oz = _mm256_add_ps(oz, _mm256_mul_ps(z, m22));

        _mm256_storeu_ps(out.x + i, ox);
        _mm256_storeu_ps(out.y + i, oy);
        _mm256_storeu_ps(out.z + i, oz);
    }
#endif

#if ABS_SIMD_BACKEND == ABS_SIMD_SSE2
    const __m128 m00 = _mm_set1_ps(m.m[0][0]);
    const __m128 m01 = _mm_set1_ps(m.m[0][1]);
    const __m128 m02 = _mm_set1_ps(m.m[0][2]);

    const __m128 m10 = _mm_set1_ps(m.m[1][0]);
    const __m128 m11 = _mm_set1_ps(m.m[1][1]);
    const __m128 m12 = _mm_set1_ps(m.m[1][2]);

    const __m128 m20 = _mm_set1_ps(m.m[2][0]);
    const __m128 m21 = _mm_set1_ps(m.m[2][1]);
    const __m128 m22 = _mm_set1_ps(m.m[2][2]);

    for (; i + 4 <= count; i += 4) {
        const __m128 x = _mm_loadu_ps(in.x + i);
        const __m128 y = _mm_loadu_ps(in.y + i);
        const __m128 z = _mm_loadu_ps(in.z + i);

        __m128 ox = _mm_mul_ps(x, m00);
        ox = _mm_add_ps(ox, _mm_mul_ps(y, m01));
        ox = _mm_add_ps(ox, _mm_mul_ps(z, m02));

        __m128 oy = _mm_mul_ps(x, m10);
        oy = _mm_add_ps(oy, _mm_mul_ps(y, m11));
        oy = _mm_add_ps(oy, _mm_mul_ps(z, m12));

        __m128 oz = _mm_mul_ps(x, m20);
        oz = _mm_add_ps(oz, _mm_mul_ps(y, m21));
        oz = _mm_add_ps(oz, _mm_mul_ps(z, m22));

        _mm_storeu_ps(out.x + i, ox);
        _mm_storeu_ps(out.y + i, oy);
        _mm_storeu_ps(out.z + i, oz);
    }
#endif

#if ABS_SIMD_BACKEND == ABS_SIMD_NEON
    const float32x4_t m00 = vdupq_n_f32(m.m[0][0]);
    const float32x4_t m01 = vdupq_n_f32(m.m[0][1]);
    const float32x4_t m02 = vdupq_n_f32(m.m[0][2]);

    const float32x4_t m10 = vdupq_n_f32(m.m[1][0]);
    const float32x4_t m11 = vdupq_n_f32(m.m[1][1]);
    const float32x4_t m12 = vdupq_n_f32(m.m[1][2]);

    const float32x4_t m20 = vdupq_n_f32(m.m[2][0]);
    const float32x4_t m21 = vdupq_n_f32(m.m[2][1]);
    const float32x4_t m22 = vdupq_n_f32(m.m[2][2]);

    for (; i + 4 <= count; i += 4) {
        const float32x4_t x = vld1q_f32(in.x + i);
        const float32x4_t y = vld1q_f32(in.y + i);
        const float32x4_t z = vld1q_f32(in.z + i);

        float32x4_t ox = vmulq_f32(x, m00);
        ox = vmlaq_f32(ox, y, m01);
        ox = vmlaq_f32(ox, z, m02);

        float32x4_t oy = vmulq_f32(x, m10);
        oy = vmlaq_f32(oy, y, m11);
        oy = vmlaq_f32(oy, z, m12);

        float32x4_t oz = vmulq_f32(x, m20);
        oz = vmlaq_f32(oz, y, m21);
        oz = vmlaq_f32(oz, z, m22);

        vst1q_f32(out.x + i, ox);
        vst1q_f32(out.y + i, oy);
        vst1q_f32(out.z + i, oz);
    }
#endif

    for (; i < count; ++i) {
        out.x[i] = m.m[0][0] * in.x[i] + m.m[0][1] * in.y[i] + m.m[0][2] * in.z[i];
        out.y[i] = m.m[1][0] * in.x[i] + m.m[1][1] * in.y[i] + m.m[1][2] * in.z[i];
        out.z[i] = m.m[2][0] * in.x[i] + m.m[2][1] * in.y[i] + m.m[2][2] * in.z[i];
    }
}

inline void vec3_dot_batch(const vec3* a, const vec3* b, float* out, std::size_t count) {
    for (std::size_t i = 0; i < count; ++i) {
        out[i] = vec3_dot(a[i], b[i]);
    }
}

inline void vec3_cross_batch(const vec3* a, const vec3* b, vec3* out, std::size_t count) {
    for (std::size_t i = 0; i < count; ++i) {
        out[i] = vec3_cross(a[i], b[i]);
    }
}

inline void mat4_transform_points(const mat4& m, const vec3* in, vec3* out, std::size_t count) {
    for (std::size_t i = 0; i < count; ++i) {
        out[i] = mat4_mul_vec3_point(m, in[i]);
    }
}

inline void mat4_transform_directions(const mat4& m,
                                      const vec3* in,
                                      vec3* out,
                                      std::size_t count) {
    for (std::size_t i = 0; i < count; ++i) {
        out[i] = mat4_mul_vec3_direction(m, in[i]);
    }
}
