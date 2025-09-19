SRCMODULES = crypto.c generator.c monocypher.c solver.c
OBJMODULES = $(SRCMODULES:.c=.o)
LIBS = libgmp.a libmpz.a
CFLAGS = -Wall -Wextra -std=c99 -pedantic -O2
LDFLAGS = -static -s

%.o: %.c %.h
	$(CC) $(CFLAGS) -c $< -o $@

timelock: main.c $(OBJMODULES) $(LIBS)
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@

clean:
	rm -f *.o timelock
