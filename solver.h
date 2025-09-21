#ifndef SOLVER_H
#define SOLVER_H

#include <stdint.h>

void solve_puzzle(char **secret_key_str, uint64_t squarings,
                  const char *modulo_str);

uint64_t run_benchmark(const char *modulo_str, unsigned seconds);

#endif
