add_program(cat
    cat.c
    ILIBS common
)

add_program(cp
    cp.c
    ILIBS common
)

add_program(crc32
    crc32.c
    LIBS zvb_crc
    ILIBS common
)

add_program(du
    du.c
    ILIBS common
)

add_program(echo
    echo.c
    ILIBS common
)

add_program(hexdump
    hexdump.c
    ILIBS common
)

add_program(ls
    ls.c
    ILIBS common
)

add_program(mkdir
    mkdir.c
    ILIBS common
)

add_program(mv
    mv.c
    ILIBS common
)

add_program(rm
    rm.c
    ILIBS common
)

add_program(setfont
    setfont.c
    ILIBS common
)

add_program(touch
    touch.c
    ILIBS common
)

add_program(tree
    tree.c
    ILIBS common
)
