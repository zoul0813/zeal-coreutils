# Called by: find_package(COREUTILS REQUIRED)

set(libraries core conio keyboard windows)

foreach(lib ${libraries})
    add_library(${lib} INTERFACE IMPORTED)

    set_target_properties(${lib} PROPERTIES
        IMPORTED_LIBNAME "${lib}"
        INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_LIST_DIR}/../libsrc/lib${lib}"
        INTERFACE_LINK_DIRECTORIES "${CMAKE_CURRENT_LIST_DIR}/../lib"
    )
endforeach()
