bits 64

kSYS_READ       equ 0x00
kSYS_WRITE      equ 0x01
kSYS_EXIT       equ 0x3c

kSTDOUT_FD      equ 0x01
kSTDIN_FD       equ 0x00

kBUFFER_SIZE    equ 32

kNEW_LINE       equ 0x0a

global _start

OutputI64:
        push rbp        ;
        mov rbp, rsp    ; enter

        sub rsp, 8 + kBUFFER_SIZE

        mov rcx, [rbp - 8]      ; i64 arg
        lea rdi, [rbp - 9]             ; right of the buffer
        inc rdi

        mov r8, 0

        jmp .while_loop_out
.another_cycle_out:
        dec rdi         ; move though buffer
        mov rdx, 0      ; rdx_rax = 0_rcx for div
        mov rax, rcx    ;
        mov r9, 10     ;
        div r9
        add rdx, '0'    ; rem += '0'
        mov [rdi], dl
        mov rcx, rax
        inc r8
.while_loop_out:
        test rcx, rcx
        jne .another_cycle_out

        ; add al, '0'
        ; mov [rdi], al

        mov rax, kSYS_WRITE
        mov rsi, rdi
        mov rdi, kSTDOUT_FD
        mov rdx, r8
        and rsp, -16
        syscall

        mov rsp, rbp
        pop rbp
        ret

InputI64:
        push rbp
        mov rbp, rsp

        sub rsp, kBUFFER_SIZE

        mov rax, kSYS_READ
        mov rdi, kSTDIN_FD
        mov rsi, rsp
        mov rdx, kBUFFER_SIZE
        and rsp, -16
        syscall

        mov rdi, rbp
        sub rdi, kBUFFER_SIZE

        mov r8, 0
        mov rsi, 0

        jmp .while_loop_in
.another_cycle_in:
        sub rsi, '0'
        imul r8, r8, 10
        add r8, rsi
        inc rdi
.while_loop_in:
        mov sil, [rdi]
        cmp rsi, kNEW_LINE
        jne .another_cycle_in

        mov rax, r8

        mov rsp, rbp
        pop rbp
        ret

Exit:
        mov rax, kSYS_EXIT
        mov rdi, 0
        syscall