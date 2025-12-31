add_library(core STATIC
    itoa.c
    itoa_pad.c
    u8tohex.c

    date.c

    max.asm
    min.asm
    clamp.asm

    mem_cpy.asm
    mem_set.asm
    mem_cmp.c

    put_c.c
    put_s.c
    put_u8.c
    put_u16.c
    put_hex.c

    str_cat.c
    str_chr.c
    str_chrr.c
    str_cmp.c
    str_cpy.asm
    str_ends_with.c
    str_len.asm
    str_pos.c
)

target_include_directories(core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_options(core PRIVATE
    $<$<COMPILE_LANGUAGE:C>:--nostdlib>
)