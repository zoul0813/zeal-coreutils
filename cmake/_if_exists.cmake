if (NOT DEFINED COREUTILS_IF_EXISTS_FILE OR NOT DEFINED COREUTILS_IF_EXISTS_ACTION)
    message(FATAL_ERROR "coreutils_if_exists script mode requires COREUTILS_IF_EXISTS_FILE and COREUTILS_IF_EXISTS_ACTION")
endif()

if (EXISTS "${COREUTILS_IF_EXISTS_FILE}")
    execute_process(
        COMMAND ${COREUTILS_IF_EXISTS_ACTION}
        RESULT_VARIABLE _action_result
    )

    if (NOT _action_result EQUAL 0)
        message(FATAL_ERROR "coreutils_if_exists action failed for '${COREUTILS_IF_EXISTS_FILE}'")
    endif()
endif()
