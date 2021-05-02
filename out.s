###### start ASM ######
	.file	"foo.txt"
	.text
	.globl	main
	.type	main, @function
main:
.LFB0:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	movl	$10, %ebx
	movl	%ebx, -16(%rbp)
	movl	$2, %ebx
	movl	%ebx, -20(%rbp)
	movl	$0, %ebx
	movl	%ebx, -24(%rbp)
.L3: 	movl	-20(%rbp), %ebx
	movl	$2, %r10d
	cmpl	%ebx, %r10d
	jne	.L5
.L2: 	nop
	movl	-16(%rbp), %ebx
	movl	$5, %r10d
	cmpl	%ebx, %r10d
	jle	.L5
.L4: 	movl	-16(%rbp), %ebx
	movl	$1, %r10d
	movl	%ebx, %r11d
	subl	%r10d, %r11d
	movl	%r11d, -16(%rbp)
	movl	-24(%rbp), %ebx
	movl	$123, %r10d
	movl	%ebx, %r11d
	addl	%r10d, %r11d
	movl	%r11d, -24(%rbp)
	jmp	.L3
.L5: 	nop
	movl	$0, %ebx
	movl	%ebx, %eax
	movl	%ebx, -4(%rbp)
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE0:
	.size	main, .-main

