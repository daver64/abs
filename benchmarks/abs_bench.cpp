#include <chrono>
#include <cstring>
#include <cstdio>
#include <cstdint>
#include <random>
#include <string>
#include <vector>

#include "abs/abs.hpp"

namespace {

using Clock = std::chrono::steady_clock;

struct bench_result {
    const char* name;
    double total_ms;
    double ns_per_element;
    double checksum;
};

template <typename Fn>
bench_result run_benchmark(const char* name,
                           std::size_t elements,
                           std::size_t iterations,
                           const Fn& fn,
                           volatile double& sink) {
    const auto start = Clock::now();
    double checksum = 0.0;

    for (std::size_t i = 0; i < iterations; ++i) {
        checksum += fn();
    }

    const auto end = Clock::now();
    const auto total_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
    const double total_ms = static_cast<double>(total_ns) / 1000000.0;
    const double ns_per_element = static_cast<double>(total_ns) / static_cast<double>(elements * iterations);

    sink += checksum;
    return {name, total_ms, ns_per_element, checksum};
}

void print_result(const bench_result& r) {
    std::printf("%-34s total=%10.3f ms  ns/elem=%8.3f  checksum=%12.3f\n",
                r.name,
                r.total_ms,
                r.ns_per_element,
                r.checksum);
}

void print_csv_header() {
    std::printf("mode,backend,test,total_ms,ns_per_element,checksum,elements,iterations\n");
}

void print_csv_result(const char* mode,
                      const char* backend,
                      const bench_result& r,
                      std::size_t elements,
                      std::size_t iterations) {
    std::printf("%s,%s,%s,%.6f,%.6f,%.6f,%zu,%zu\n",
                mode,
                backend,
                r.name,
                r.total_ms,
                r.ns_per_element,
                r.checksum,
                elements,
                iterations);
}

void print_usage(const char* exe) {
    std::printf("Usage: %s [--csv] [--csv-no-header] [--mode <label>]\n", exe);
}

} // namespace

int main(int argc, char** argv) {
    bool csv_output = false;
    bool csv_no_header = false;
    std::string mode_label = "runtime";

    for (int i = 1; i < argc; ++i) {
        if (std::strcmp(argv[i], "--csv") == 0) {
            csv_output = true;
            continue;
        }
        if (std::strcmp(argv[i], "--csv-no-header") == 0) {
            csv_no_header = true;
            continue;
        }
        if (std::strcmp(argv[i], "--mode") == 0) {
            if (i + 1 >= argc) {
                std::fprintf(stderr, "--mode requires a value\n");
                print_usage(argv[0]);
                return 2;
            }
            mode_label = argv[++i];
            continue;
        }
        if (std::strcmp(argv[i], "--help") == 0 || std::strcmp(argv[i], "-h") == 0) {
            print_usage(argv[0]);
            return 0;
        }
        std::fprintf(stderr, "Unknown argument: %s\n", argv[i]);
        print_usage(argv[0]);
        return 2;
    }

    constexpr std::size_t count = 1u << 20;
    constexpr std::size_t iterations = 24;

    if (!csv_output) {
        std::printf("abs benchmark\n");
        std::printf("SIMD backend: %s\n", abs_simd_backend_name());
        std::printf("elements: %zu, iterations: %zu\n\n", count, iterations);
    }

    std::mt19937 rng(1337u);
    std::uniform_real_distribution<float> dist(-100.0f, 100.0f);

    std::vector<vec3> aos_a(count);
    std::vector<vec3> aos_b(count);
    std::vector<vec3> aos_out(count);
    std::vector<float> dot_out(count);

    std::vector<float> ax(count), ay(count), az(count);
    std::vector<float> bx(count), by(count), bz(count);
    std::vector<float> ox(count), oy(count), oz(count);

    for (std::size_t i = 0; i < count; ++i) {
        const vec3 a{dist(rng), dist(rng), dist(rng)};
        const vec3 b{dist(rng), dist(rng), dist(rng)};

        aos_a[i] = a;
        aos_b[i] = b;

        ax[i] = a.x;
        ay[i] = a.y;
        az[i] = a.z;

        bx[i] = b.x;
        by[i] = b.y;
        bz[i] = b.z;
    }

    const mat4 m = mat4_trs({2.0f, -3.0f, 4.0f},
                            quat_from_axis_angle({0.2f, 1.0f, 0.3f}, 0.9f),
                            {1.2f, 0.7f, 2.1f});

    const vec3_soa_const_view soa_a{ax.data(), ay.data(), az.data()};
    const vec3_soa_const_view soa_b{bx.data(), by.data(), bz.data()};
    const vec3_soa_view soa_out{ox.data(), oy.data(), oz.data()};

    volatile double sink = 0.0;

    const auto dot_aos = run_benchmark(
        "dot_batch AoS",
        count,
        iterations,
        [&]() {
            vec3_dot_batch(aos_a.data(), aos_b.data(), dot_out.data(), count);
            double sum = 0.0;
            for (std::size_t i = 0; i < count; i += 1024) {
                sum += dot_out[i];
            }
            return sum;
        },
        sink);

    const auto dot_soa = run_benchmark(
        "dot_batch SoA",
        count,
        iterations,
        [&]() {
            vec3_dot_batch_soa(soa_a, soa_b, dot_out.data(), count);
            double sum = 0.0;
            for (std::size_t i = 0; i < count; i += 1024) {
                sum += dot_out[i];
            }
            return sum;
        },
        sink);

    const auto cross_aos = run_benchmark(
        "cross_batch AoS",
        count,
        iterations,
        [&]() {
            vec3_cross_batch(aos_a.data(), aos_b.data(), aos_out.data(), count);
            double sum = 0.0;
            for (std::size_t i = 0; i < count; i += 1024) {
                sum += aos_out[i].x + aos_out[i].y + aos_out[i].z;
            }
            return sum;
        },
        sink);

    const auto cross_soa = run_benchmark(
        "cross_batch SoA",
        count,
        iterations,
        [&]() {
            vec3_cross_batch_soa(soa_a, soa_b, soa_out, count);
            double sum = 0.0;
            for (std::size_t i = 0; i < count; i += 1024) {
                sum += ox[i] + oy[i] + oz[i];
            }
            return sum;
        },
        sink);

    const auto transform_aos = run_benchmark(
        "mat4_transform_points AoS",
        count,
        iterations,
        [&]() {
            mat4_transform_points(m, aos_a.data(), aos_out.data(), count);
            double sum = 0.0;
            for (std::size_t i = 0; i < count; i += 1024) {
                sum += aos_out[i].x + aos_out[i].y + aos_out[i].z;
            }
            return sum;
        },
        sink);

    const auto transform_soa = run_benchmark(
        "mat4_transform_points SoA",
        count,
        iterations,
        [&]() {
            mat4_transform_points_soa_affine(m, soa_a, soa_out, count);
            double sum = 0.0;
            for (std::size_t i = 0; i < count; i += 1024) {
                sum += ox[i] + oy[i] + oz[i];
            }
            return sum;
        },
        sink);

    if (csv_output) {
        if (!csv_no_header) {
            print_csv_header();
        }
        const char* backend = abs_simd_backend_name();
        print_csv_result(mode_label.c_str(), backend, dot_aos, count, iterations);
        print_csv_result(mode_label.c_str(), backend, dot_soa, count, iterations);
        print_csv_result(mode_label.c_str(), backend, cross_aos, count, iterations);
        print_csv_result(mode_label.c_str(), backend, cross_soa, count, iterations);
        print_csv_result(mode_label.c_str(), backend, transform_aos, count, iterations);
        print_csv_result(mode_label.c_str(), backend, transform_soa, count, iterations);
    } else {
        print_result(dot_aos);
        print_result(dot_soa);
        print_result(cross_aos);
        print_result(cross_soa);
        print_result(transform_aos);
        print_result(transform_soa);

        std::printf("\nanti-opt sink: %.3f\n", sink);
    }

    return sink == 0.123456 ? 1 : 0;
}
