#ifndef GEN_PUZZLE_H
#define GEN_PUZZLE_H

#include <stdint.h>

void gen_puzzle(char **secret_key_str, char **modulo_str,
                uint64_t squarings);

#endif
