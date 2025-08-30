#ifndef CRYPTO_H
#define CRYPTO_H

#include <stddef.h>
#include <stdint.h>

int encrypt(uint8_t *cipher_text, uint8_t mac[16],
            const uint8_t *message, size_t len,
            const char *secret_key);

void decrypt(uint8_t *plain_text, uint8_t mac[16],
             const uint8_t *cipher_text, size_t len,
             const char *secret_key);

#endif
