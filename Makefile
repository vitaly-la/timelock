all:
	gcc -march=native -O3 solver.c -lgmp -o solver

clean:
	rm -f ./solver
