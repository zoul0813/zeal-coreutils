if (NOT DEFINED COREUTILS_DIR)
    set(COREUTILS_DIR "${CMAKE_CURRENT_LIST_DIR}")
endif()

if (NOT DEFINED COREUTILS_IF_EXISTS_SCRIPT)
    set(COREUTILS_IF_EXISTS_SCRIPT "${COREUTILS_DIR}/_if_exists.cmake")
endif()

function(coreutils_if_exists out_var file)
    if (ARGC LESS 3)
        message(FATAL_ERROR "coreutils_if_exists requires: out_var, file, and at least one action argument")
    endif()

    set(_commands "${${out_var}}")
    set(_action ${ARGN})
    list(JOIN _action ";" _action_serialized)
    string(REPLACE ";" "\\;" _action_serialized "${_action_serialized}")

    list(APPEND _commands
        COMMAND ${CMAKE_COMMAND}
            -DCOREUTILS_IF_EXISTS=ON
            "-DCOREUTILS_IF_EXISTS_FILE=${file}"
            "-DCOREUTILS_IF_EXISTS_ACTION=${_action_serialized}"
            -P ${COREUTILS_IF_EXISTS_SCRIPT}
    )
    set(${out_var} "${_commands}" PARENT_SCOPE)
endfunction()

function(coreutils_add_post_build_artifacts target_name)
    cmake_parse_arguments(
        COREUTILS_POST_BUILD
        ""
        "TARGET;OUT_DIR;DEBUG_DIR;STRIP_BIN_EXTENSION"
        ""
        ${ARGN}
    )

    if (NOT COREUTILS_POST_BUILD_TARGET)
        set(COREUTILS_POST_BUILD_TARGET "${target_name}_bin")
    endif()

    if (NOT COREUTILS_POST_BUILD_OUT_DIR)
        set(COREUTILS_POST_BUILD_OUT_DIR "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
    endif()

    if (NOT COREUTILS_POST_BUILD_DEBUG_DIR)
        set(COREUTILS_POST_BUILD_DEBUG_DIR "${CMAKE_SOURCE_DIR}/debug")
    endif()

    if (NOT DEFINED COREUTILS_POST_BUILD_STRIP_BIN_EXTENSION)
        set(COREUTILS_POST_BUILD_STRIP_BIN_EXTENSION ON)
    endif()

    set(_post_build_commands
        COMMAND ${CMAKE_COMMAND} -E make_directory ${COREUTILS_POST_BUILD_DEBUG_DIR}
    )

    set(COREUTILS_DEBUG_ARTIFACT_EXTENSIONS cdb map ihx)
    foreach(ext IN ITEMS ${COREUTILS_DEBUG_ARTIFACT_EXTENSIONS})
        set(src "${COREUTILS_POST_BUILD_OUT_DIR}/${target_name}.${ext}")
        set(dst "${COREUTILS_POST_BUILD_DEBUG_DIR}/${target_name}.${ext}")
        coreutils_if_exists(
            _post_build_commands
            "${src}"
            ${CMAKE_COMMAND} -E rename "${src}" "${dst}"
        )
    endforeach()

    if (COREUTILS_POST_BUILD_STRIP_BIN_EXTENSION)
        coreutils_if_exists(
            _post_build_commands
            "${COREUTILS_POST_BUILD_OUT_DIR}/${target_name}.bin"
            ${CMAKE_COMMAND} -E rename
            "${COREUTILS_POST_BUILD_OUT_DIR}/${target_name}.bin"
            "${COREUTILS_POST_BUILD_OUT_DIR}/${target_name}"
        )
    endif()

    add_custom_command(TARGET ${COREUTILS_POST_BUILD_TARGET} POST_BUILD
        ${_post_build_commands}
        COMMENT "Moving debug files to debug directory"
        VERBATIM
    )
endfunction()
