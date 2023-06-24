cmake_minimum_required(VERSION 3.21)

project(nes-emu)

if(${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
    add_compile_options("/std:c++latest")
elseif(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
    set(CMAKE_CXX_STANDARD 20)
    set(CMAKE_CXX_STANDARD_REQUIRED 1)
endif()

include(${CMAKE_SOURCE_DIR}/cmake/CMakeUtils.cmake)

set_variables()
setup_conan()

add_subdirectory(vendor)
add_subdirectory(src)

add_executable(${CMAKE_PROJECT_NAME} main.cpp)
target_link_libraries(${CMAKE_PROJECT_NAME} PUBLIC nes-emu-lib)