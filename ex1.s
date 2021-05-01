###### start ASM ######
        .file   "foo.txt"
        .text
        .comm   GLOBAL,4
        .globl  clara
        .type   clara, @function
clara:
.LFB0:
        .cfi_startproc
        endbr64
        pushq   %rbp
        .cfi_def_cfa_offset 16
        .cfi_offset 6, -16
        movq    %rsp, %rbp
sss
        movl    %edi, -16(%rbp)
sss
        movl    %esi, -20(%rbp)
sss
        movl    %edx, -24(%rbp)
sss
        movl    %ecx, -28(%rbp)
sss
        movl    %r8d, -32(%rbp)
        movl    -20(%rbp), %r11d
        movl    %r10d, %r12d
        addl    %r11d, %r12d
        movl    r12d, %eax
        popq    %rbp
        .cfi_def_cfa 7, 8
        ret
        .cfi_endproc
.LFE0:
        .size   clara, .-clara
        .globl  main
        .type   main, @function
main:
.LFB1:
        .cfi_startproc
        endbr64
        pushq   %rbp
        .cfi_def_cfa_offset 16
        .cfi_offset 6, -16
        movq    %rsp, %rbp
        movl    $69, %r10d
        movl    %r10d, GLOBAL(%rip)
        movl    $1, %r10d
        movl    %r10d, -24(%rbp)
        movl    $2, %r10d
        movl    %r10d, -32(%rbp)
        movl    GLOBAL(%rip), %r10d
        movl    %r10d, -36(%rbp)
        movl    %rbp, -8(%rsp)
        movl    -24(%rbp), %r10d
        movl    %r10d, %edi
        movl    -32(%rbp), %r10d
        movl    %r10d, %esi
        movl    $1, %r10d
        movl    %r10d, %edx
        movl    $2, %r10d
        movl    %r10d, %ecx
        movl    $3, %r10d
        movl    %r10d, %r8d
        call    clara
        movl    %eax, %ebx
        movl    %ebx, -36(%rbp)
        movl    -36(%rbp), %ebx
        movl    ebx, %eax
        movl    %ebx, -4(%rbp)
        leave
        .cfi_def_cfa 7, 8
        ret
        .cfi_endproc
.LFE1:
        .size   main, .-main