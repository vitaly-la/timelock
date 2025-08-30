#include "generator.h"

#include "gmp.h"

enum { half_key_size = 1536 };

void generate_puzzle(char **secret_key_str, char **modulo_str,
                     uint64_t squarings)
{
    gmp_randstate_t state;
    gmp_randinit_default(state);

    mpz_t rop, p, q, modulo, p1, q1, phi,
          exp, two, squarings_mpz, secret_key;
    mpz_inits(rop, p, q, modulo, p1, q1, phi,
              exp, two, squarings_mpz, secret_key, NULL);
    mpz_set_ui(two, 2);
    mpz_set_ui(squarings_mpz, squarings);

    mpz_rrandomb(rop, state, half_key_size);
    mpz_nextprime(p, rop);

    mpz_rrandomb(rop, state, half_key_size);
    mpz_nextprime(q, rop);

    mpz_mul(modulo, p, q);

    mpz_sub_ui(p1, p, 1);
    mpz_sub_ui(q1, q, 1);
    mpz_mul(phi, p1, q1);

    mpz_powm(exp, two, squarings_mpz, phi);
    mpz_powm(secret_key, two, exp, modulo);

    *secret_key_str = mpz_get_str(NULL, 10, secret_key);
    *modulo_str     = mpz_get_str(NULL, 10, modulo);

    mpz_clears(rop, p, q, modulo, p1, q1, phi,
               exp, two, squarings_mpz, secret_key, NULL);
    gmp_randclear(state);
}
