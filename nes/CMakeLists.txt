cmake_minimum_required(VERSION 3.21)

project(nes-emu)

if(${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
    add_compile_options("/std:c++latest")
elseif(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
    set(CMAKE_CXX_STANDARD 20)
    set(CMAKE_CXX_STANDARD_REQUIRED 1)
endif()

add_subdirectory(src)

add_subdirectory(vendor)

add_executable(${CMAKE_PROJECT_NAME} main.cpp)

target_link_libraries(${CMAKE_PROJECT_NAME} PUBLIC imgui-lib nes-emu-lib)