CFLAGS = -O3
OBJS = data.o text.o

a.out: $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) $(OBJS)

data.o: xaa xab xac

clean:
	rm -f a.out $(OBJS)
