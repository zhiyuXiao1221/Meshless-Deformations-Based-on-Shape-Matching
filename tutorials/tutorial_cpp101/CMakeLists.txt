cmake_minimum_required(VERSION 2.8.7)

#initialize project
set(proj_name cpp101)
project(${proj_name})
set(src_files main.cpp)

#include Eigen
include_directories(${PROJECT_SOURCE_DIR}/../../ext/eigen)

#add executable
add_executable(${proj_name} ${src_files})
