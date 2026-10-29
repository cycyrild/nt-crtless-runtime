.intel_syntax noprefix

.section .text$runtime_start,"xr"

.global start
.extern Bootstrap
.extern DllMain

# DLL entry point trampoline that preserves the loader-provided arguments
# and initial stack pointer across initialization before entering DllMain.

.equ DLL_PROCESS_ATTACH, 1

.equ SHADOW_SPACE_SIZE, 32
.equ SAVED_ARGS_SIZE,    24
.equ STACK_FRAME_SIZE,   SHADOW_SPACE_SIZE + SAVED_ARGS_SIZE

start:
    sub     rsp, STACK_FRAME_SIZE

    mov     [rsp + SHADOW_SPACE_SIZE],      rcx      # Preserve initial HINSTANCE
    mov     [rsp + SHADOW_SPACE_SIZE + 8],  rdx      # Preserve initial DWORD reason
    mov     [rsp + SHADOW_SPACE_SIZE + 16], r8       # Preserve initial LPVOID reserved

    # Run bootstrap only when the DLL is attached to the process.
    cmp     edx, DLL_PROCESS_ATTACH
    jne     .Lskip_bootstrap

    call    Bootstrap

.Lskip_bootstrap:
    mov     rcx, [rsp + SHADOW_SPACE_SIZE]           # Restore initial HINSTANCE
    mov     rdx, [rsp + SHADOW_SPACE_SIZE + 8]       # Restore initial DWORD reason
    mov     r8,  [rsp + SHADOW_SPACE_SIZE + 16]      # Restore initial LPVOID reserved
    lea     r9,  [rsp + STACK_FRAME_SIZE]

    add     rsp, STACK_FRAME_SIZE
    jmp     DllMain
