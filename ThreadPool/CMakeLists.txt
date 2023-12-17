cmake_minimum_required(VERSION 3.10)
project(ThreadPool)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_EXTENSIONS OFF)
# 查找线程库
find_package(Threads REQUIRED)

add_executable(ThreadPool main.cpp)

target_include_directories(ThreadPool PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(ThreadPool PUBLIC Threads::Threads)