add_subdirectory(imgui)

target_include_directories(imgui-lib PUBLIC imgui/include)
conan_target_link_libraries(imgui-lib)