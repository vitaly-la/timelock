OBJ     := main.o generator.o solver.o crypto.o monocypher.o
LIB     := libgmp.a libmpz.a
TARGET  := timelock
CFLAGS  := -std=c99 -O3 -Wpedantic -pedantic-errors -Werror -Wall -Wextra
CFLAGS  += -Waggregate-return -Wbad-function-cast -Wcast-align -Wcast-qual
CFLAGS  += -Wfloat-equal -Wformat=2 -Wmissing-declarations
CFLAGS  += -Wmissing-include-dirs -Wmissing-prototypes -Wnested-externs
CFLAGS  += -Wpointer-arith -Wredundant-decls -Wsequence-point -Wshadow
CFLAGS  += -Wstrict-prototypes -Wswitch -Wundef -Wunreachable-code
CFLAGS  += -Wunused-parameter -Wwrite-strings
LDFLAGS := -static -s

${TARGET}: ${OBJ}
	${CC} ${LDFLAGS} ${OBJ} ${LIB} -o ${TARGET}

%.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@

clean:
	rm -f ${OBJ} ${TARGET}
