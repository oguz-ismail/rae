#include <asm/unistd.h>

	.globl	_start

	.text
#if __linux__ && __aarch64__
#define def(f) \
	.globl	f; \
f:; \
	mov	w8, __NR_##f; \
	b	.Lsyscall

.Lsyscall:
	svc	0
	cmn	x0, 4095
	b.cc	0f
	neg	w0, w0
	adrp	x1, errno
	str	w0, [x1, :lo12:errno]
	mov	w0, -1
0:	ret

_start:
	bl	main
	mov	w8, __NR_exit
	svc	0
#elif __linux__ && __amd64__
#define def(f) \
	.globl	f; \
f:; \
	mov	%rcx, %r10; \
	mov	$__NR_##f, %eax; \
	jmp	.Lsyscall

.Lsyscall:
	syscall
	cmp	$-4095, %rax
	jb	0f
	neg	%eax
	mov	%eax, errno
	mov	$-1, %eax
0:	ret

_start:
	and	$-16, %rsp
	call	main
	mov	%eax, %edi
	mov	$__NR_exit, %eax
	syscall
#elif __linux__ && __arm__
#define def(f) \
	.globl	f; \
f:; \
	push	{r7}; \
	mov	r7, __NR_##f; \
	b	.Lsyscall

.Lsyscall:
	swi	0
	pop	{r7}
	cmn	r0, 4096
	bxcc	lr
	movw	r1, :lower16:errno
	movt	r1, :upper16:errno
	str	r0, [r1]
	mov	r0, -1
	bx	lr

_start:
	bl	main
	mov	r7, __NR_exit
	swi	0
#elif __linux__ && __i386__
#define def(f) \
	.globl	f; \
f:; \
	push	%ebx; \
	push	%ecx; \
	push	%edx; \
	push	%esi; \
	push	%edi; \
	push	%ebp; \
	mov	28(%esp), %ebx; \
	mov	32(%esp), %ecx; \
	mov	36(%esp), %edx; \
	mov	40(%esp), %esi; \
	mov	44(%esp), %edi; \
	mov	48(%esp), %ebp; \
	mov	$__NR_##f, %eax; \
	jmp	.Lsyscall

.Lsyscall:
	int	$128
	pop	%ebp
	pop	%edi
	pop	%esi
	pop	%edx
	pop	%ecx
	pop	%ebx
	cmp	$-4095, %eax
	jc	0f
	neg	%eax
	mov	%eax, errno
	mov	$-1, %eax
0:	ret

_start:
	call	main
	mov	%eax, %ebx
	mov	$__NR_exit, %eax
	int	$128

#else
#error target not supported
#endif

def(ioctl)
def(read)
def(write)
