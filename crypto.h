#ifndef CRYPTO_H
#define CRYPTO_H

#include <stddef.h>
#include <stdint.h>

void encrypt(uint8_t *cipher_text, uint8_t nonce[24], uint8_t mac[16],
             const uint8_t *message, size_t len,
             const char *secret_key);

#endif
