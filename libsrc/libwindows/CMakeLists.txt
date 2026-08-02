add_library(windows STATIC
    dialogs.c
    vram.asm
    globals.asm
    window_title.asm
    window.asm
    window_active.asm
    window_columns.asm
    window_gotox.asm
    window_gotoy.asm
    window_gotoxy.asm
    window_clrscr.asm
    window_clreol.asm
    window_save.asm
    window_restore.asm
    mirror.asm
    window_wherex.asm
    window_wherey.asm
    window_putc.asm
    window_puts.asm
    banners.c
    address_bc.asm
    fill8.asm
)

target_include_directories(windows PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(windows PRIVATE $ENV{ZVB_SDK_PATH}/include)
target_link_libraries(windows PRIVATE core)
target_compile_options(windows PRIVATE
    $<$<COMPILE_LANGUAGE:C>:--nostdlib>
)
