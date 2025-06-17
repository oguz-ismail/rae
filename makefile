CFLAGS += -O3
bin = a.out
objs = data.o text.o

.c.o:
	$(CC) $(CFLAGS) $(CPPFLAGS) -c -o $@ $<

all: $(bin)

$(bin): $(objs)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(objs)

data.o: xaa xab xac

clean:
	rm -f $(bin) $(objs:data.o=)

distclean: clean
	rm -f data.o
