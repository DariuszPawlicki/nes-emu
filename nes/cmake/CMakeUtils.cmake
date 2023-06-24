macro(set_variables)
    set(CONANFILE_PATH ${CMAKE_SOURCE_DIR}/conanfile.py)
endmacro()

macro(setup_conan)
    download_cmake_conan()
    include(${CMAKE_BINARY_DIR}/conan/conan.cmake)
    conan_cmake_run(
            CONANFILE
            ${CONANFILE_PATH}
            PROFILE
            debug
            BASIC_SETUP
            BUILD
            missing
    )
    include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
    conan_basic_setup(TARGETS)
endmacro()

function(download_cmake_conan)
    if(NOT EXISTS ${CMAKE_BINARY_DIR}/conan/conan.cmake)
        message(STATUS "Downloading cmake-conan...")
        file(
                DOWNLOAD
                https://raw.githubusercontent.com/conan-io/cmake-conan/develop/conan.cmake
                ${CMAKE_BINARY_DIR}/conan/conan.cmake
        )
    endif()
endfunction()