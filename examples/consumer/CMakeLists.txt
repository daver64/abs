cmake_minimum_required(VERSION 3.16)
project(abs_consumer_example LANGUAGES CXX)

find_package(abs CONFIG REQUIRED)

add_executable(abs_consumer_example main.cpp)
target_link_libraries(abs_consumer_example PRIVATE abs::abs)
