#ifndef SOLVER_H
#define SOLVER_H

#include <stdint.h>

void solve(char **secret_key_str, uint64_t squarings,
           const char *modulo_str);

#endif
