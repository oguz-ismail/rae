include makefile

CPPFLAGS += -DNOLIBC
CFLAGS += -fno-builtin -fno-pie -fno-stack-protector -fno-unwind-tables \
	-fno-asynchronous-unwind-tables -fomit-frame-pointer
LDFLAGS += -static -no-pie -nostdlib -Wl,--build-id=none -Wl,-z,noexecstack \
	-Wl,-z,norelro
objs += crt.o

.SUFFIXES: .S

.S.o:
	$(CC) $(CFLAGS) $(CPPFLAGS) -c -o $@ $<

$(bin): crt.o
