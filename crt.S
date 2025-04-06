#include <asm/unistd.h>

	.globl	errno
	.globl	_start

	.bss
errno:
	.word

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
#else
#error target not supported
#endif

def(ioctl)
def(read)
def(write)
