#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gmp.h>

int main(int argc, char *argv[]) {
    unsigned char digits[1024] = {0};
    mp_limb_t quotient[96] = {0};
    mp_limb_t sqr[96] = {0};
    mp_limb_t x[48] = {0};
    mp_limb_t n[48] = {0};
    uint64_t i = 0;
    uint64_t t = 0;
    mp_size_t mp_len = 0;
    size_t len = 0;

    len = strlen(argv[1]);
    for (i = 0; i < len; ++i) {
        digits[i] = argv[1][i] - '0';
    }
    mpn_set_str(x, digits, len, 10);

    t = atoll(argv[2]);

    len = strlen(argv[3]);
    for (i = 0; i < len; ++i) {
        digits[i] = argv[3][i] - '0';
    }
    mp_len = mpn_set_str(n, digits, len, 10);

    for (i = 0; i < t; ++i) {
        mpn_sqr(sqr, x, 48);
        mpn_tdiv_qr(quotient, x, 0, sqr, 96, n, mp_len);
    }

    len = mpn_get_str(digits, 10, x, 48);
    for (i = 0; i < len; ++i) {
        digits[i] += '0';
    }
    digits[len] = '\0';
    puts(digits);

    return 0;
}
