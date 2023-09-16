cmake_minimum_required(VERSION 3.10)
project(ToTpAuth)

set(CMAKE_CXX_STANDARD 17)
set(CRYPTOPP_SOURCES ${PROJECT_SOURCE_DIR}/cryptopp880)
aux_source_directory(src SRC)
add_subdirectory(cryptopp-cmake_)
include_directories(include)
add_executable(ToTpAuth ${SRC} )
target_link_libraries(ToTpAuth cryptopp)


