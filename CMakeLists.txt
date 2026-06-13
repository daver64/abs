cmake_minimum_required(VERSION 3.16)

project(abs VERSION 0.1.0 LANGUAGES CXX)

set(ABS_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(ABS_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(ABS_VERSION_PATCH ${PROJECT_VERSION_PATCH})
set(ABS_VERSION ${PROJECT_VERSION})

option(ABS_BUILD_DEMO "Build the abs demo executable" ON)
option(ABS_BUILD_BENCHMARKS "Build abs benchmark executable" OFF)
option(ABS_BUILD_SCENE_EXAMPLE "Build abs scene example executable" ON)
set(ABS_SIMD_MODE "SCALAR" CACHE STRING "SIMD backend: SCALAR, AUTO, SSE2, AVX2, or NEON")
set_property(CACHE ABS_SIMD_MODE PROPERTY STRINGS SCALAR AUTO SSE2 AVX2 NEON)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

add_library(abs INTERFACE)
add_library(abs::abs ALIAS abs)

target_include_directories(abs
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
target_compile_features(abs INTERFACE cxx_std_17)

if(ABS_SIMD_MODE STREQUAL "SCALAR")
    target_compile_definitions(abs INTERFACE ABS_FORCE_SCALAR=1)
elseif(ABS_SIMD_MODE STREQUAL "AUTO")
    target_compile_definitions(abs INTERFACE ABS_SIMD_AUTO=1)
elseif(ABS_SIMD_MODE STREQUAL "SSE2")
    target_compile_definitions(abs INTERFACE ABS_FORCE_SSE2=1)
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(abs INTERFACE -msse2)
    endif()
elseif(ABS_SIMD_MODE STREQUAL "AVX2")
    target_compile_definitions(abs INTERFACE ABS_FORCE_AVX2=1)
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(abs INTERFACE -mavx2)
    endif()
elseif(ABS_SIMD_MODE STREQUAL "NEON")
    target_compile_definitions(abs INTERFACE ABS_FORCE_NEON=1)
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang" AND CMAKE_SYSTEM_PROCESSOR MATCHES "armv7|arm")
        target_compile_options(abs INTERFACE -mfpu=neon)
    endif()
else()
    message(FATAL_ERROR "Unsupported ABS_SIMD_MODE='${ABS_SIMD_MODE}'. Use SCALAR, AUTO, SSE2, AVX2, or NEON.")
endif()

if(ABS_BUILD_DEMO)
    add_executable(abs_demo src/abs.cpp)
    target_link_libraries(abs_demo PRIVATE abs::abs)
endif()

if(ABS_BUILD_BENCHMARKS)
    add_executable(abs_bench benchmarks/abs_bench.cpp)
    target_link_libraries(abs_bench PRIVATE abs::abs)
endif()

if(ABS_BUILD_SCENE_EXAMPLE)
    add_executable(abs_scene_example src/scene_example.cpp)
    target_link_libraries(abs_scene_example PRIVATE abs::abs)
endif()

install(TARGETS abs EXPORT absTargets)

install(
    DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(
    EXPORT absTargets
    FILE absTargets.cmake
    NAMESPACE abs::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/abs
)

configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/absConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/absConfig.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/abs
)

write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/absConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

install(
    FILES
        ${CMAKE_CURRENT_BINARY_DIR}/absConfig.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/absConfigVersion.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/abs
)

set(CPACK_PACKAGE_NAME "abs")
set(CPACK_PACKAGE_VENDOR "abs project")
set(CPACK_PACKAGE_CONTACT "maintainers@abs.local")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Header-only C++ vec3 math library")
set(CPACK_PACKAGE_VERSION_MAJOR ${ABS_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${ABS_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${ABS_VERSION_PATCH})
set(CPACK_PACKAGE_FILE_NAME
    "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}-${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR}"
)
set(CPACK_SOURCE_PACKAGE_FILE_NAME
    "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}-source"
)
set(CPACK_PACKAGE_DIRECTORY "${CMAKE_BINARY_DIR}")
set(CPACK_GENERATOR "TGZ;ZIP")
set(CPACK_SOURCE_GENERATOR "TGZ;ZIP")
set(CPACK_SOURCE_IGNORE_FILES
    "/build/"
    "/install/"
    "/examples/consumer/build/"
    "/.git/"
    "/.github/"
)

include(CPack)
