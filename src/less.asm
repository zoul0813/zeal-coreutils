; SPDX-FileCopyrightText: 2023 Zeal 8-bit Computer <contact@zeal8bit.com>
;
; SPDX-License-Identifier: Apache-2.0

;--------------------------------------------------------
; Public variables in this module
;--------------------------------------------------------
    .globl _main

;--------------------------------------------------------
; Constants
;--------------------------------------------------------
FILENAME_SIZE = 16
PAGE3_ADDR = 0x8000
PAGE3_SIZE = 16384
COMMAND_QUIT = 'q'        ; KB_KEY_Q
COMMAND_SCROLL_UP = 0xa0   ; KB_UP_ARROW (adjust as needed)
COMMAND_SCROLL_DOWN = 0xa1 ; KB_DOWN_ARROW (adjust as needed)
DEV_STDOUT = 0
DEV_STDIN = 1
O_RDONLY = 0
CMD_CLEAR_SCREEN = 6
CMD_SET_CURSOR_XY = 4
CMD_GET_AREA = 1
KB_CMD_SET_MODE = 0
KB_MODE_RAW = 0

AREA_END_T = 4

.macro SYSCALL
    rst 0x8
.endm

.macro OPEN
    ld l, #2
    SYSCALL
.endm

.macro CLOSE
    ld l, #3
    SYSCALL
.endm

.macro READ
    ld l, #0
    SYSCALL
.endm

.macro WRITE
    ld l, #1
    SYSCALL
.endm

.macro IOCTL
    ld l, #7
    SYSCALL
.endm

.macro EXIT
    ld e, a
    ld d, #0
    ret
.endm


;--------------------------------------------------------
; ram data
;--------------------------------------------------------
    .area _DATA
file_fd:
    .ds 1
screen_area:
    .ds AREA_END_T
buffer_filled:
    .ds 2
buffer_from:
    .ds 2
init_static_buffer_from:
    .ds 2
init_static_buffer_size:
    .ds 1

; A static buffer that can be used across the commands implementation
init_static_buffer::
    .ds 1024
init_static_buffer_end:
INIT_STATIC_BUFFER_LEN = init_static_buffer_end - init_static_buffer

;--------------------------------------------------------
; code
;--------------------------------------------------------
    .area _TEXT

_kernel_config:
	ld	de, (#0x0004)
	ret

; "less" command main function
; Parameters:
;       HL - ARGC
;       DE - ARGV
; Returns:
;       A - 0 on success
_main::
    ; Check that argc is 1
    ld a, l
    cp #1
    jp nz, _less_usage
    ; Retrieve the filename given as a parameter
    inc de
    inc de ; skip the first pointer?
    ex de, hl ; HL is now ARGV

    ld c, l
    ld b, h
    ; Filepath in BC now, open the file
    ; BC - Path to file
    ; H - Flags
    ld h, #O_RDONLY
    OPEN ; OPEN()
    or a
    ; In this case, A is negative if an error occurred
    jp M, open_error

    ; Save the file descriptor
    ld (file_fd), a

    ; Get the number of characters on screen
    call get_screen_characters_count
    or a
    jp NZ, ioctl_error

    ; Set the input to RAW
    call set_stdin_mode
    jp NZ, ioctl_error

    ; Reset the print buffer by simulating a CR
    call print_char_cr

    ; Save the number of characters on screen
    ld a, (file_fd)
    ld h, a
    ; FIXME: Use the global buffer instead of the third page, to allow files
    ; bigger than 16KB
    ld de, #PAGE3_ADDR
    ld (buffer_from), de
    ld bc, #PAGE3_SIZE
    READ ; READ()
    or a
    jp NZ, read_error

    ; Save the number of characters read (file size)
    ld hl, #PAGE3_ADDR
    add hl, bc
    ld (buffer_filled), hl
_less_clear_and_print:
    ; Clear the screen and print the characters from the file
    call clear_set_cursor
    or a
    jp NZ, ioctl_error
    call print_buffer
_less_wait_command:
    ; Set the cursor to the bottom of the screen and listen on the keyboard
    call listen_on_input
    cp #COMMAND_QUIT
    jp Z, _less_end
    cp #COMMAND_SCROLL_UP
    jr Z, _less_scroll_up
    cp #COMMAND_SCROLL_DOWN
    jr Z, _less_scroll_down
    jr _less_wait_command
_less_scroll_up:
    ; Scroll the screen up
    call scroll_up
    jr _less_clear_and_print
_less_scroll_down:
    ; Scroll the screen down
    call scroll_down
    jr _less_clear_and_print
_less_end:
    ; On exit, clear the screen and set the cursor back
    call clear_set_cursor
    ld a, (file_fd)
    ld h, a
    ; Close the opened file
    CLOSE ; CLOSE()
    xor a

    EXIT

_less_usage:
    ; S_WRITE3 #DEV_STDOUT, str_usage, (str_usage_end - str_usage)
    ld h, #DEV_STDOUT
    ld de, #str_usage
    ld bc, #STR_USAGE_LEN
    WRITE
    ld a, #0
    EXIT

error_print:
    push af
    ld h, #DEV_STDOUT
    WRITE
    pop af
    ret


ioctl_error:
    ld bc, #STR_IOCTL_ERR_LEN
    ld de, #str_ioctl_err
    jr print_error
open_error:
    neg
    ld bc, #STR_OPEN_ERR_LEN
    ld de, #str_open_err
    jr print_error
read_error:
    ld b, a
    CLOSE ; CLOSE()
    ld a, b
    ld bc, #STR_READ_ERR_LEN
    ld de, #str_read_err
print_error:
    call error_print
    ld de, #3
    EXIT

set_stdin_mode:
    ld h, #DEV_STDIN
    ld c, #KB_CMD_SET_MODE
    ld e, #KB_MODE_RAW
    IOCTL ; IOCTL()
    ret

clear_set_cursor:
    ld h, #DEV_STDOUT
    ld c, #CMD_CLEAR_SCREEN
    IOCTL ; IOCTL()
    or a
    ret NZ
    ld c, #CMD_SET_CURSOR_XY
    ld de, #0
    IOCTL ; IOCTL()
    ret

get_screen_characters_count:
    ld de, #screen_area
    ld c, #CMD_GET_AREA
stdout_ioctl:
    ld h, #DEV_STDOUT
    IOCTL ; IOCTL()
    or a
    ret

get_current_buffer_size:
    ld de, (buffer_from)
    ld hl, (buffer_filled)
    or a
    sbc hl, de
    ld b, h
    ld c, l
    ret

scroll_down:
    call get_current_buffer_size
    ret Z
    ld hl, (buffer_from)
    ld a, #0x0a  ; '\n'
    cpir
    ret NZ
    ld (buffer_from), hl
    ret

scroll_up:
    ld de, (buffer_from)
    ld hl, #(-PAGE3_ADDR)
    or a
    adc hl, de
    ret Z
    ex de, hl
    ld b, d
    ld c, e
    dec hl
    dec bc
    ld a, b
    or c
    jr Z, _scroll_up_store
    dec hl
    dec bc
    ld a, b
    or c
    jr Z, _scroll_up_store
    ld a, #0x0a  ; '\n'
    cpdr
    jr NZ, _scroll_up_store
    inc hl
    inc hl
_scroll_up_store:
    ld (buffer_from), hl
    ret

print_buffer:
    call get_current_buffer_size
    ret Z
    ld hl, #0
    ex de, hl
_print_buffer_loop:
    ld a, (hl)
    inc hl
    dec bc
    cp #0x0d  ; '\r'
    jr Z, _print_buffer_cr
    cp #0x0a  ; '\n'
    jr Z, _print_buffer_nl
    call print_char
    inc d
    ld a, (screen_area)
    cp d
    jr NZ, _print_buffer_check_bc
    ld a, #0x0a  ; '\n'
    cpir
    ret NZ
    jr _print_buffer_nl_no_print
_print_buffer_cr:
    ld d, #0
    call print_char_cr
    jr _print_buffer_check_bc
_print_buffer_nl:
    call print_char
_print_buffer_nl_no_print:
    ld d, #0
    inc e
    call print_flush
    ld a, (screen_area + 1)
    dec a
    cp e
    ret Z
_print_buffer_check_bc:
    ld a, b
    or c
    jr NZ, _print_buffer_loop
    ret

print_char_cr:
    push hl
    ld hl, #init_static_buffer
    ld (init_static_buffer_from), hl
    pop hl
    ret

print_char:
    push hl
    ld hl, (init_static_buffer_from)
    ld (hl), a
    inc hl
    ld (init_static_buffer_from), hl
    pop hl
    ret

print_flush:
    push bc
    push de
    push hl
    ld hl, (init_static_buffer_from)
    ld de, #init_static_buffer
    or a
    sbc hl, de
    ld b, h
    ld c, l
    ld de, #init_static_buffer
    ld (init_static_buffer_from), de
    ; S_WRITE1(DEV_STDOUT)
    ld h, #DEV_STDOUT
    WRITE

    pop hl
    pop de
    pop bc
    ret

listen_on_input:
    ld a, (screen_area + 1)
    dec a
    ld d, #0
    ld e, a
    ld c, #CMD_SET_CURSOR_XY
    call stdout_ioctl
    or a
    ret NZ
    ld de, #init_static_buffer
    ld bc, #INIT_STATIC_BUFFER_LEN
    ld h, #DEV_STDIN
    READ ; READ()
    ld a, (de)
    ret

;--------------------------------------------------------
; String constants
;--------------------------------------------------------
str_usage:
    .ascii "usage: less <path_to_file>"
    .db 0x0a
    .db 0x00
str_usage_end:
STR_USAGE_LEN = str_usage_end - str_usage

str_open_err:
    .ascii "open error: "
    .db 0x00
str_open_err_end:
STR_OPEN_ERR_LEN = str_open_err_end - str_open_err

str_read_err:
    .ascii "read error: "
    .db 0x00
str_read_err_end:
STR_READ_ERR_LEN = str_read_err_end - str_read_err


str_ioctl_err:
    .ascii "ioctl error: "
    .db 0x00
str_ioctl_err_end:
STR_IOCTL_ERR_LEN = str_ioctl_err_end - str_ioctl_err

    .area _TEXT
