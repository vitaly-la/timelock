WARN += -Wpedantic -pedantic-errors
WARN += -Werror
WARN += -Wall
WARN += -Wextra
WARN += -Waggregate-return
WARN += -Wbad-function-cast
WARN += -Wcast-align
WARN += -Wcast-qual
WARN += -Wfloat-equal
WARN += -Wformat=2
WARN += -Wmissing-declarations
WARN += -Wmissing-include-dirs
WARN += -Wmissing-prototypes
WARN += -Wnested-externs
WARN += -Wpointer-arith
WARN += -Wredundant-decls
WARN += -Wsequence-point
WARN += -Wshadow
WARN += -Wstrict-prototypes
WARN += -Wswitch
WARN += -Wundef
WARN += -Wunreachable-code
WARN += -Wunused-but-set-parameter
WARN += -Wwrite-strings

all:
	cc -std=c99 -O3 ${WARN} -c main.c generator.c crypto.c solver.c \
	                           monocypher.c
	cc -static main.o generator.o crypto.o solver.o monocypher.o \
	           libgmp.a libmpz.a -o timelock

clean:
	rm -f *.o timelock
