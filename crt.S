.globl errno
.globl syscall

.bss
errno:
	.word

.text
syscall:
#if __linux__ && __aarch64__
	mov x8, x0
	mov x0, x1
	mov x1, x2
	mov x2, x3
	mov x3, x4
	mov x4, x5
	mov x5, x6
	mov x6, x7
	svc 0
	cmn x0, 4095
	b.cc 0f
	neg w0, w0
	adrp x1, errno
	str w0, [x1, :lo12:errno]
	mov w0, -1
0:	ret
#elif __linux__ && __amd64__
	mov %rdi, %rax
	mov %rsi, %rdi
	mov %rdx, %rsi
	mov %rcx, %rdx
	mov %r8, %r10
	mov %r9, %r8
	mov 8(%rsp), %r9
	syscall
	cmp $-4095, %rax
	jb 0f
	neg %eax
	mov %eax, errno
	mov $-1, %eax
0:	ret
#else
#error target not supported
#endif
