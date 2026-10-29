.intel_syntax noprefix

# PE entry point trampoline that preserves the initial PEB pointer and stack
# pointer across early initialization, then forwards them to NtProcessStartup.

.section .text$runtime_start,"xr"

.global start
.extern Bootstrap
.extern NtProcessStartup

.equ SHADOW_SPACE_SIZE, 32
.equ SAVED_ARGS_SIZE,    8
.equ STACK_FRAME_SIZE,   SHADOW_SPACE_SIZE + SAVED_ARGS_SIZE

start:
    sub     rsp, STACK_FRAME_SIZE

    mov     [rsp + SHADOW_SPACE_SIZE], rcx       # Preserve initial PEB pointer

    call    Bootstrap

    mov     rcx, [rsp + SHADOW_SPACE_SIZE]       # Restore initial PEB pointer
    lea     rdx, [rsp + STACK_FRAME_SIZE]        # Recover original RSP

    add     rsp, STACK_FRAME_SIZE
    jmp     NtProcessStartup
