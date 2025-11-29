cmake_minimum_required(VERSION 3.16)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)

set(ZOS_TOOLCHAIN sdcc)
include($ENV{ZOS_PATH}/cmake/zos_init.cmake)

zos_use(zvb)
find_package(ZVB_SDK REQUIRED)

# When using CMake as a build system, it's possible to mix ASM and C in the compilation
project(zeal_modem C ASM)

include_directories(${CMAKE_SOURCE_DIR}/include)

add_subdirectory(libsrc/libcore)
add_subdirectory(libsrc/libconio)
add_subdirectory(libsrc/libkeyboard)
add_subdirectory(libsrc/libwindows)

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
            # target_link_libraries(${target_name} PUBLIC "-k ${CMAKE_LIBRARY_OUTPUT_DIRECTORY} -l ${lib}.lib")
            zos_link_libraries(${target_name} PRIVATE ${lib})
        endforeach()
    endif()

    zos_add_outputs(${target_name})

    add_custom_command(TARGET ${target_name}_bin POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_SOURCE_DIR}/debug
        COMMAND ${CMAKE_COMMAND} -E rename
            ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${target_name}.cdb
            ${CMAKE_SOURCE_DIR}/debug/${target_name}.cdb
        COMMAND ${CMAKE_COMMAND} -E rename
            ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${target_name}.map
            ${CMAKE_SOURCE_DIR}/debug/${target_name}.map
        COMMAND ${CMAKE_COMMAND} -E rename
            ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${target_name}.ihx
            ${CMAKE_SOURCE_DIR}/debug/${target_name}.ihx
        COMMAND ${CMAKE_COMMAND} -E rename
            ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${target_name}.bin
            ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${target_name}
        COMMENT "Moving debug files to debug directory"
        VERBATIM
    )
endfunction()

add_subdirectory(src)