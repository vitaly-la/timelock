#include "crypto.h"

#include <stdlib.h>
#include <string.h>

#include "monocypher.h"

void encrypt(uint8_t *cipher_text, uint8_t nonce[24], uint8_t mac[16],
             const uint8_t *message, size_t len,
             const char *secret_key)
{
    uint8_t hash[32];
    crypto_blake2b(hash, sizeof(hash),
                   (const uint8_t*)secret_key, strlen(secret_key));

    arc4random_buf(nonce, 24);
    crypto_aead_lock(cipher_text, mac, hash, nonce,
                     NULL, 0, message, len);
}

void decrypt(uint8_t *plain_text, uint8_t nonce[24], uint8_t mac[16],
             const uint8_t *cipher_text, size_t len,
             const char *secret_key)
{
    uint8_t hash[32];
    crypto_blake2b(hash, sizeof(hash),
                   (const uint8_t*)secret_key, strlen(secret_key));

    crypto_aead_unlock(plain_text, mac, hash, nonce,
                       NULL, 0, cipher_text, len);
}
