cmake_minimum_required(VERSION 3.16)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)

set(ZOS_TOOLCHAIN sdcc)
include($ENV{ZOS_PATH}/cmake/zos_init.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/coreutils_init.cmake)

zos_use(zvb)
find_package(ZVB_SDK REQUIRED)

# When using CMake as a build system, it's possible to mix ASM and C in the compilation
project(coreutils C ASM)

include_directories(${CMAKE_SOURCE_DIR}/include)

add_subdirectory(libsrc/libcore)
add_subdirectory(libsrc/libconio)
add_subdirectory(libsrc/libkeyboard)
add_subdirectory(libsrc/libwindows)
add_subdirectory(libsrc/libansi)

function(link_prebuilt_lib target_name lib)
    target_link_options(${target_name} PRIVATE
        "SHELL:-k ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}"
        "SHELL:-l ${lib}.lib"
    )
    target_include_directories(${target_name} PRIVATE
        "${CMAKE_SOURCE_DIR}/libsrc/lib${lib}"
    )
endfunction()

function(add_program target_name)
    cmake_parse_arguments(ARG "" "" "SRCS;LIBS;ILIBS" ${ARGN})

    add_executable(${target_name} ${ARG_UNPARSED_ARGUMENTS})
    target_include_directories(${target_name} PRIVATE $ENV{ZVB_SDK_PATH}/include)

    target_compile_options(${target_name} PRIVATE
        $<$<COMPILE_LANGUAGE:C>:--nostdlib>
    )

    if(ARG_LIBS)
        target_link_libraries(${target_name} PRIVATE ${ARG_LIBS})
    endif()

    if(ARG_ILIBS)
        foreach(lib IN LISTS ARG_ILIBS)
            link_prebuilt_lib(${target_name} ${lib})
        endforeach()
    endif()

    zos_add_outputs(${target_name})

    coreutils_add_post_build_artifacts(${target_name})
    set_property(GLOBAL APPEND PROPERTY COREUTILS_BIN_TARGETS "${target_name}_bin")
endfunction()

add_subdirectory(src)

get_property(coreutils_bin_targets GLOBAL PROPERTY COREUTILS_BIN_TARGETS)
add_custom_target(bins DEPENDS ${coreutils_bin_targets})
