#include "solver.h"

#include "gmp.h"

void solve(char **secret_key_str, uint64_t squarings,
           const char *modulo_str)
{
    mpz_t secret_key, modulo;
    mpz_inits(secret_key, modulo, NULL);

    mpz_set_ui(secret_key, 2);
    mpz_init_set_str(modulo, modulo_str, 10);

    for (uint64_t i = 0; i < squarings; ++i) {
        mpz_powm_ui(secret_key, secret_key, 2, modulo);
    }

    *secret_key_str = mpz_get_str(NULL, 10, secret_key);

    mpz_clears(secret_key, modulo, NULL);
}
