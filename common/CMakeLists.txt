add_library(common STATIC
    itoa.c
    itoa_pad.c
    u8tohex.c

    max.c
    min.c

    mem_cpy.c
    mem_set.c

    put_c.c
    put_s.c
    put_sn.c

    str_cat.c
    str_cmp.c
    str_cpy.c
    str_ends_with.c
    str_len.c
    str_pos.c
)
target_compile_options(common PRIVATE --nostdlib)