#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gmp.h>

int main(int argc, char *argv[]) {
    mp_limb_t x[64] = {2};
    mp_limb_t n[64] = {0};
    mp_limb_t sqr[64] = {0};
    mp_limb_t quotient[64] = {0};
    unsigned char digits[1024] = {0};
    uint64_t i = 0;
    uint64_t t = 0;
    size_t len = 0;

    t = atoll(argv[1]);
    len = strlen(argv[2]);
    for (i = 0; i < len; ++i) {
        digits[i] = argv[2][i] - '0';
    }
    mpn_set_str(n, digits, len, 10);

    for (i = 0; i < t; ++i) {
        mpn_sqr(sqr, x, 32);
        mpn_tdiv_qr(quotient, x, 0, sqr, 64, n, 32);
    }

    memset(digits, 0, sizeof(digits));
    len = mpn_get_str(digits, 10, x, 32);
    for (i = 0; i < len; ++i) {
        digits[i] += '0';
    }
    printf("%s\n", digits);

    return 0;
}
