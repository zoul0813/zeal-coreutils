add_program(cat
    cat.c
    ILIBS core
)

add_program(cp
    cp.c
    ILIBS core
)

add_program(crc32
    crc32.c
    LIBS zvb_crc
    ILIBS core
)

add_program(du
    du.c
    ILIBS core
)

add_program(echo
    echo.c
    ILIBS core
)

add_program(hexdump
    hexdump.c
    ILIBS core
)

add_program(ls
    ls.c
    ILIBS core
)

add_program(less
    less.asm
)

add_program(mkdir
    mkdir.c
    ILIBS core
)

add_program(mv
    mv.c
    ILIBS core
)

add_program(return
    return.c
    ILIBS core
)

add_program(rm
    rm.c
    ILIBS core
)

add_program(setfont
    setfont.c
    ILIBS core
)

add_program(stat
    stat.c
    ILIBS core
)

add_program(touch
    touch.c
    ILIBS core
)

add_program(tree
    tree.c
    ILIBS core
)

add_program(xfer
    xfer.asm
)