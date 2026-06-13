# abs

Small C++ math library for 3D work covering vectors, quaternions, matrices, polar/spherical coordinate conversion, and SIMD-friendly batch operations.

This project is packaged as an installable CMake library so other projects can consume it with `find_package`.

Design style: plain POD-like structs and free functions/operators, with C-style naming for ergonomics.

## What You Get

- Header-only library target: `abs::abs`
- Optional demo executable: `abs_demo`
- Optional scene example executable: `abs_scene_example`
- Optional SDL3 + OpenGL4 example executable: `abs_sdl3_gl4_example`
- Install/export support for downstream projects
- Stable public include path layout under `include/abs/`
- Consumer smoke-test example under `examples/consumer`
- Semantic versioning via CMake project version (`MAJOR.MINOR.PATCH`)
- Binary and source package generation via CPack (`.tar.gz` and `.zip`)
- SIMD-ready aligned type `vec4f` and optional SIMD backends
- Batch APIs for AoS and SoA data paths
- Optional benchmark executable for AoS vs SoA and SIMD comparisons

## Modules

- `vec3`: arithmetic operators, dot, cross, length, normalize, lerp
- `quat`: quaternion arithmetic, axis-angle creation, normalization, slerp/nlerp, Euler conversion
- `mat3`/`mat4`: identity, multiplication, transpose, determinant, inverse, normal-matrix, TRS composition
- polar/spherical coordinates: Cartesian conversion helpers
- `simd`: aligned `vec4f`, backend selection, SIMD-friendly math helpers
- `batch`: bulk vec3 dot/cross and matrix transforms (AoS wrappers + SoA fast path)
- `camera`: look-at view matrix and perspective/orthographic projections
- `geometry`: ray/plane/sphere/aabb/obb/triangle/frustum primitives and helpers
- `intersection`: ray, primitive overlap, and frustum culling tests
- `transform`: world/local conversions, TBN basis build, handedness conversion helpers
- `numeric`: epsilon-safe comparisons, clamping, wrap-angle, safe normalize
- `interpolation`: Hermite/Catmull-Rom/Bezier, smooth damp, critically damped spring, easing
- `sampling`: disk/sphere/hemisphere sampling plus Halton/Hammersley sequences

## SIMD Configuration

`abs` defaults to scalar mode for portability and determinism.

Configure backend at CMake configure time:

```bash
cmake -S . -B build -DABS_SIMD_MODE=SCALAR
```

Available values:

- `SCALAR` (default)
- `AUTO` (detect from compiler target features)
- `SSE2`
- `AVX2`
- `NEON`

Runtime/backend visibility from code:

```cpp
const char* backend = abs_simd_backend_name();
```

## Requirements

- CMake 3.16+
- C++17 compiler (GCC, Clang, or MSVC)

## Build

```bash
cmake -S . -B build
cmake --build build
```

## Run Demo

```bash
./build/abs_demo
```

## Run Scene Example

```bash
./build/abs_scene_example
```

This example demonstrates camera setup, frustum culling, ray-sphere/triangle intersection, barycentrics, TBN basis creation, and sampling helpers in one small flow.

## SDL3 + OpenGL4 Example

This optional example uses SDL3 with a hardware OpenGL 4 core-profile context and modern shader-based rendering (no fixed-function pipeline).
It explicitly uses the camera module (`camera_perspective`, `camera_view_matrix`, `camera_projection_matrix`) for view/projection.

Controls:

- `A` / `D`: orbit left/right (yaw)
- `W` / `S`: orbit up/down (pitch)
- `Q` / `E`: zoom in/out
- `Esc`: quit

Build with SDL3/GL4 target enabled:

```bash
cmake -S . -B build -DABS_BUILD_SDL3_GL4_EXAMPLE=ON -DABS_FETCH_SDL3=ON
cmake --build build
```

Run:

```bash
./build/abs_sdl3_gl4_example
```

Requirements:

- SDL3 development package discoverable by CMake OR enable local fetch (`-DABS_FETCH_SDL3=ON`)
- OpenGL development libraries
- GPU/driver supporting OpenGL 4+ core profile

If SDL3 is not installed system-wide, CMake will fetch SDL3 into your local build tree when `ABS_FETCH_SDL3` is enabled.

## Benchmarks

Build benchmark target:

```bash
cmake -S . -B build -DABS_BUILD_BENCHMARKS=ON -DABS_SIMD_MODE=SCALAR
cmake --build build
```

Run benchmark:

```bash
./build/abs_bench
```

CSV output mode:

```bash
./build/abs_bench --csv --mode SCALAR > benchmarks/results_scalar.csv
```

Try backend comparisons by rebuilding with different SIMD modes:

```bash
cmake -S . -B build -DABS_BUILD_BENCHMARKS=ON -DABS_SIMD_MODE=AUTO
cmake --build build
./build/abs_bench
```

Run all variants and write one consolidated CSV:

```bash
./scripts/run_bench_variants.sh
```

The script writes `benchmarks/results.csv` and attempts modes:

- `SCALAR`
- `AUTO`
- `SSE2`
- `AVX2`
- `NEON`

You can also pass a custom subset:

```bash
./scripts/run_bench_variants.sh SCALAR AUTO AVX2
```

## Install The Package

Install to a local prefix (example: `./install`):

```bash
cmake -S . -B build -DCMAKE_INSTALL_PREFIX="$PWD/install"
cmake --build build
cmake --install build
```

This installs:

- headers into `install/include`
- CMake package files into `install/lib/cmake/abs`

## Versioning

The project follows semantic versioning from the CMake project version in `CMakeLists.txt`:

```cmake
project(abs VERSION 0.1.0 LANGUAGES CXX)
```

Update this value as follows:

- `MAJOR`: breaking API changes
- `MINOR`: backward-compatible features
- `PATCH`: backward-compatible fixes

## Create Redistributable Packages

After configuring/building, generate binary archives:

```bash
cpack --config build/CPackConfig.cmake
```

Generate source archives:

```bash
cpack --config build/CPackSourceConfig.cmake
```

Artifacts are created in `build/` as `.tar.gz` and `.zip` files.

## Public Headers

Preferred include path:

```cpp
#include "abs/abs.hpp"
```

Module-level includes are also available:

```cpp
#include "abs/abs.hpp"
#include "abs/vec3.hpp"
#include "abs/quaternion.hpp"
#include "abs/matrix.hpp"
#include "abs/polar.hpp"
#include "abs/simd.hpp"
#include "abs/camera.hpp"
#include "abs/geometry.hpp"
#include "abs/intersection.hpp"
#include "abs/transform.hpp"
#include "abs/numeric.hpp"
#include "abs/interpolation.hpp"
#include "abs/sampling.hpp"
#include "abs/batch.hpp"
```

Backward-compatible include remains available:

```cpp
#include "absinc.h"
```

## Use In Another CMake Project

In the consumer project's `CMakeLists.txt`:

```cmake
find_package(abs CONFIG REQUIRED)

add_executable(my_app main.cpp)
target_link_libraries(my_app PRIVATE abs::abs)
```

Configure the consumer with the install prefix in `CMAKE_PREFIX_PATH`:

```bash
cmake -S . -B build -DCMAKE_PREFIX_PATH="/path/to/abs/install"
cmake --build build
```

## Consumer Example

A complete external-consumer example is included in `examples/consumer`.

```bash
cmake -S examples/consumer -B examples/consumer/build -DCMAKE_PREFIX_PATH="$PWD/install"
cmake --build examples/consumer/build
./examples/consumer/build/abs_consumer_example
```

## CI

GitHub Actions workflow `ci` in `.github/workflows/ci.yml` validates:

- main project configure/build
- install to local prefix
- consumer configure/build/run using `find_package(abs CONFIG REQUIRED)`

## Legacy Header

```cpp
#include "absinc.h"
```

## Example API

```cpp
vec3 a{1.0f, 2.0f, 3.0f};
vec3 b{4.0f, 5.0f, 6.0f};

vec3 sum = a + b;
vec3 diff = a - b;
vec3 scaled = a * 2.0f;

float d = vec3_dot(a, b);
vec3 n = vec3_normalize(a);

quat q = quat_from_axis_angle({0.0f, 1.0f, 0.0f}, 1.57079632679f);
vec3 turned = quat_rotate_vec3(q, {1.0f, 0.0f, 0.0f});

mat4 model = mat4_trs({1.0f, 2.0f, 3.0f}, q, {2.0f, 2.0f, 2.0f});
vec3 world = mat4_mul_vec3_point(model, {1.0f, 0.0f, 0.0f});

spherical_coord s = spherical_from_vec3({1.0f, 1.0f, 1.0f});
vec3 back = spherical_to_vec3(s);

vec4f p = vec4f_from_vec3(a, 1.0f);
vec4f r = vec4f_add(p, vec4f_set(1.0f, 1.0f, 1.0f, 0.0f));
float dp = vec4f_dot4(p, r);

mat4 view = mat4_look_at({0.0f, 2.0f, 6.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f});
mat4 proj = mat4_perspective(1.0471975512f, 16.0f / 9.0f, 0.1f, 1000.0f);
frustum fr = frustum_from_view_projection(mat4_mul(proj, view));

sphere s1{{0.0f, 0.0f, 0.0f}, 1.0f};
bool visible = intersects_frustum_sphere(fr, s1);

float angle = wrap_angle_pi(7.0f);
float eased = ease_in_out_cubic(0.35f);

vec3 h = sample_uniform_hemisphere(0.42f, 0.73f);
```

## Batch API Sketch

```cpp
// AoS wrapper path
vec3_dot_batch(points_a, points_b, out_dot, count);

// SoA fast path
vec3_soa_const_view sa{ax, ay, az};
vec3_soa_const_view sb{bx, by, bz};
vec3_dot_batch_soa(sa, sb, out_dot, count);
```
# abs
