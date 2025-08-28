#ifndef CRYPTO_H
#define CRYPTO_H

#include <stddef.h>

void encrypt(const char *message, size_t len, const char *secret_key);

#endif
