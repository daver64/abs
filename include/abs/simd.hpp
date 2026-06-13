#pragma once

#include <cstddef>

#include "abs/vec3.hpp"

#define ABS_SIMD_NONE 0
#define ABS_SIMD_SSE2 1
#define ABS_SIMD_AVX2 2
#define ABS_SIMD_NEON 3

#ifndef ABS_FORCE_SCALAR
#define ABS_FORCE_SCALAR 0
#endif

#ifndef ABS_FORCE_SSE2
#define ABS_FORCE_SSE2 0
#endif

#ifndef ABS_FORCE_AVX2
#define ABS_FORCE_AVX2 0
#endif

#ifndef ABS_FORCE_NEON
#define ABS_FORCE_NEON 0
#endif

#ifndef ABS_SIMD_AUTO
#define ABS_SIMD_AUTO 0
#endif

#ifndef ABS_SIMD_BACKEND
#if ABS_FORCE_SCALAR
#define ABS_SIMD_BACKEND ABS_SIMD_NONE
#elif ABS_FORCE_AVX2
#if defined(__AVX2__)
#define ABS_SIMD_BACKEND ABS_SIMD_AVX2
#else
#define ABS_SIMD_BACKEND ABS_SIMD_NONE
#endif
#elif ABS_FORCE_SSE2
#if defined(__SSE2__)
#define ABS_SIMD_BACKEND ABS_SIMD_SSE2
#else
#define ABS_SIMD_BACKEND ABS_SIMD_NONE
#endif
#elif ABS_FORCE_NEON
#if defined(__ARM_NEON) || defined(__ARM_NEON__)
#define ABS_SIMD_BACKEND ABS_SIMD_NEON
#else
#define ABS_SIMD_BACKEND ABS_SIMD_NONE
#endif
#elif ABS_SIMD_AUTO
#if defined(__AVX2__)
#define ABS_SIMD_BACKEND ABS_SIMD_AVX2
#elif defined(__SSE2__)
#define ABS_SIMD_BACKEND ABS_SIMD_SSE2
#elif defined(__ARM_NEON) || defined(__ARM_NEON__)
#define ABS_SIMD_BACKEND ABS_SIMD_NEON
#else
#define ABS_SIMD_BACKEND ABS_SIMD_NONE
#endif
#else
#define ABS_SIMD_BACKEND ABS_SIMD_NONE
#endif
#endif

#if ABS_SIMD_BACKEND == ABS_SIMD_AVX2 || ABS_SIMD_BACKEND == ABS_SIMD_SSE2
#include <immintrin.h>
#endif

#if ABS_SIMD_BACKEND == ABS_SIMD_NEON
#include <arm_neon.h>
#endif

struct alignas(16) vec4f {
    float x;
    float y;
    float z;
    float w;
};

inline const char* abs_simd_backend_name() {
#if ABS_SIMD_BACKEND == ABS_SIMD_AVX2
    return "AVX2";
#elif ABS_SIMD_BACKEND == ABS_SIMD_SSE2
    return "SSE2";
#elif ABS_SIMD_BACKEND == ABS_SIMD_NEON
    return "NEON";
#else
    return "SCALAR";
#endif
}

inline vec4f vec4f_set(float x, float y, float z, float w) {
    return {x, y, z, w};
}

inline vec4f vec4f_from_vec3(const vec3& v, float w = 0.0f) {
    return {v.x, v.y, v.z, w};
}

inline vec3 vec4f_to_vec3(const vec4f& v) {
    return {v.x, v.y, v.z};
}

inline vec4f vec4f_add(const vec4f& a, const vec4f& b) {
#if ABS_SIMD_BACKEND == ABS_SIMD_AVX2 || ABS_SIMD_BACKEND == ABS_SIMD_SSE2
    const __m128 va = _mm_loadu_ps(&a.x);
    const __m128 vb = _mm_loadu_ps(&b.x);
    const __m128 vr = _mm_add_ps(va, vb);
    vec4f out;
    _mm_storeu_ps(&out.x, vr);
    return out;
#elif ABS_SIMD_BACKEND == ABS_SIMD_NEON
    const float32x4_t va = vld1q_f32(&a.x);
    const float32x4_t vb = vld1q_f32(&b.x);
    const float32x4_t vr = vaddq_f32(va, vb);
    vec4f out;
    vst1q_f32(&out.x, vr);
    return out;
#else
    return {a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w};
#endif
}

inline vec4f vec4f_sub(const vec4f& a, const vec4f& b) {
#if ABS_SIMD_BACKEND == ABS_SIMD_AVX2 || ABS_SIMD_BACKEND == ABS_SIMD_SSE2
    const __m128 va = _mm_loadu_ps(&a.x);
    const __m128 vb = _mm_loadu_ps(&b.x);
    const __m128 vr = _mm_sub_ps(va, vb);
    vec4f out;
    _mm_storeu_ps(&out.x, vr);
    return out;
#elif ABS_SIMD_BACKEND == ABS_SIMD_NEON
    const float32x4_t va = vld1q_f32(&a.x);
    const float32x4_t vb = vld1q_f32(&b.x);
    const float32x4_t vr = vsubq_f32(va, vb);
    vec4f out;
    vst1q_f32(&out.x, vr);
    return out;
#else
    return {a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w};
#endif
}

inline vec4f vec4f_mul_scalar(const vec4f& v, float s) {
#if ABS_SIMD_BACKEND == ABS_SIMD_AVX2 || ABS_SIMD_BACKEND == ABS_SIMD_SSE2
    const __m128 vv = _mm_loadu_ps(&v.x);
    const __m128 vs = _mm_set1_ps(s);
    const __m128 vr = _mm_mul_ps(vv, vs);
    vec4f out;
    _mm_storeu_ps(&out.x, vr);
    return out;
#elif ABS_SIMD_BACKEND == ABS_SIMD_NEON
    const float32x4_t vv = vld1q_f32(&v.x);
    const float32x4_t vs = vdupq_n_f32(s);
    const float32x4_t vr = vmulq_f32(vv, vs);
    vec4f out;
    vst1q_f32(&out.x, vr);
    return out;
#else
    return {v.x * s, v.y * s, v.z * s, v.w * s};
#endif
}

inline float vec4f_dot3(const vec4f& a, const vec4f& b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

inline float vec4f_dot4(const vec4f& a, const vec4f& b) {
#if ABS_SIMD_BACKEND == ABS_SIMD_AVX2 || ABS_SIMD_BACKEND == ABS_SIMD_SSE2
    const __m128 va = _mm_loadu_ps(&a.x);
    const __m128 vb = _mm_loadu_ps(&b.x);
    const __m128 vm = _mm_mul_ps(va, vb);
    alignas(16) float temp[4];
    _mm_storeu_ps(temp, vm);
    return temp[0] + temp[1] + temp[2] + temp[3];
#elif ABS_SIMD_BACKEND == ABS_SIMD_NEON
    const float32x4_t va = vld1q_f32(&a.x);
    const float32x4_t vb = vld1q_f32(&b.x);
    const float32x4_t vm = vmulq_f32(va, vb);
    alignas(16) float temp[4];
    vst1q_f32(temp, vm);
    return temp[0] + temp[1] + temp[2] + temp[3];
#else
    return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
#endif
}

static_assert(alignof(vec4f) >= 16, "vec4f must be at least 16-byte aligned");
static_assert(sizeof(vec4f) == 16, "vec4f must be exactly 16 bytes");
