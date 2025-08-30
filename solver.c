#include "solver.h"

#include <stdio.h>
#include <string.h>

#include "gmp.h"

void solve_puzzle(char **secret_key_str, uint64_t squarings,
                  const char *modulo_str)
{
    mpz_t secret_key, modulo;
    mpz_inits(secret_key, modulo, NULL);

    mpz_set_ui(secret_key, 2);
    mpz_init_set_str(modulo, modulo_str, 10);

    char prefix[] = "Progress: [";
    char progress[51] = {0};
    memset(progress, '.', 50);
    char suffix[] = "]";

    uint64_t chunk = squarings / 50;

    for (size_t i = 0; i < 50; ++i) {
        printf("\r%s%s%s", prefix, progress, suffix);
        fflush(stdout);

        for (uint64_t j = 0; j < chunk; ++j) {
            mpz_powm_ui(secret_key, secret_key, 2, modulo);
        }

        progress[i] = '#';
    }

    for (uint64_t i = 0; i < squarings % 50; ++i) {
        mpz_powm_ui(secret_key, secret_key, 2, modulo);
    }

    printf("\r%s%s%s\n", prefix, progress, suffix);

    *secret_key_str = mpz_get_str(NULL, 10, secret_key);

    mpz_clears(secret_key, modulo, NULL);
}
