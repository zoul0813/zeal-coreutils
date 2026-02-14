add_library(ansi STATIC
    ansi.c
)
target_include_directories(ansi PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(ansi PRIVATE $ENV{ZVB_SDK_PATH}/include)
target_link_libraries(ansi PRIVATE core)
target_compile_options(ansi PRIVATE
    $<$<COMPILE_LANGUAGE:C>:--nostdlib>
)