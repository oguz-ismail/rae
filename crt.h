#include <errno.h>
#include <sys/syscall.h>
#include <unistd.h>

#undef errno
extern int errno;

#define ioctl(fd, op, ...) \
	syscall(SYS_ioctl, fd, op, __VA_ARGS__)
#define read(fd, buf, count) \
	syscall(SYS_read, fd, buf, count)
#define write(fd, buf, count) \
	syscall(SYS_write, fd, buf, count)

void __attribute__((force_align_arg_pointer))
_start(void) {
	int main(void);
	syscall(SYS_exit, main());
	__builtin_unreachable();
}
