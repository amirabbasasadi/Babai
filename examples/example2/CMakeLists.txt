cmake_minimum_required(VERSION 3.0)
project(example)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-O3 -march=native")
include_directories(../../include)

add_executable(example example.cpp)
target_link_libraries(example gsl gslcblas gmp gmpxx)
