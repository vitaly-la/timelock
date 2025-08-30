#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#include "crypto.h"
#include "generator.h"
#include "solver.h"

static void print_usage(void)
{
    printf("Usage: timelock lock <squarings>|<seconds>s <filename>\n"
           "       timelock unlock <filename>\n");
}

static void lock_file(const char *path, uint64_t squarings)
{
    int fd               = 0;
    uint8_t *addr        = NULL;
    char *secret_key     = NULL;
    char *modulo         = NULL;
    uint8_t *cipher_text = NULL;
    char *new_path       = NULL;
    FILE *fp             = NULL;
    int err              = 0;

    fd = open(path, O_RDONLY);
    if (!fd) {
        printf("open failed\n");
        err = 1;
        goto cleanup;
    }

    struct stat sb;
    fstat(fd, &sb);
    uint32_t len = sb.st_size;

    addr = mmap(NULL, len, PROT_READ, MAP_PRIVATE, fd, 0);
    if (!addr) {
        printf("mmap failed\n");
        err = 1;
        goto cleanup;
    }

    gen_puzzle(&secret_key, &modulo, squarings);
    uint16_t modulo_len = strlen(modulo) + 1;

    cipher_text = malloc(len);
    uint8_t nonce[24];
    uint8_t mac[16];
    encrypt(cipher_text, nonce, mac, addr, len, secret_key);

    char ext[] = ".enc";
    new_path = malloc(strlen(path) + sizeof(ext));
    memcpy(new_path, path, strlen(path));
    memcpy(new_path + strlen(path), ext, sizeof(ext));

    fp = fopen(new_path, "w");
    if (!fp) {
        printf("fopen failed\n");
        err = 1;
        goto cleanup;
    }
    fwrite(&squarings,  sizeof(squarings),    1,             fp);
    fwrite(&modulo_len, sizeof(modulo_len),   1,             fp);
    fwrite(modulo,      sizeof(*modulo),      modulo_len,    fp);
    fwrite(nonce,       sizeof(*nonce),       sizeof(nonce), fp);
    fwrite(mac,         sizeof(*mac),         sizeof(mac),   fp);
    fwrite(&len,        sizeof(len),          1,             fp);
    fwrite(cipher_text, sizeof(*cipher_text), len,           fp);

cleanup:
    if (addr)        munmap(addr, len);
    if (fd)          close(fd);
    if (secret_key)  free(secret_key);
    if (modulo)      free(modulo);
    if (cipher_text) free(cipher_text);
    if (new_path)    free(new_path);
    if (fp)          fclose(fp);

    if (err) exit(1);
}

static void unlock_file(const char *path)
{
    FILE *fp             = NULL;
    FILE *fp2            = NULL;
    char *modulo         = NULL;
    char *secret_key     = NULL;
    char *new_path       = NULL;
    uint8_t *cipher_text = NULL;
    uint8_t *plain_text  = NULL;
    int err              = 0;

    uint64_t squarings;
    uint16_t modulo_len;
    uint8_t nonce[24];
    uint8_t mac[16];
    uint32_t len;

    fp = fopen(path, "r");
    if (!fp) {
        printf("fopen failed\n");
        err = 1;
        goto cleanup;
    }

    fread(&squarings,  sizeof(squarings),    1,             fp);
    fread(&modulo_len, sizeof(modulo_len),   1,             fp);
    modulo = malloc(modulo_len);
    fread(modulo,      sizeof(*modulo),      modulo_len,    fp);
    fread(nonce,       sizeof(*nonce),       sizeof(nonce), fp);
    fread(mac,         sizeof(*mac),         sizeof(mac),   fp);
    fread(&len,        sizeof(len),          1,             fp);
    cipher_text = malloc(len);
    fread(cipher_text, sizeof(*cipher_text), len,           fp);

    solve(&secret_key, squarings, modulo);
    plain_text = malloc(len);
    decrypt(plain_text, nonce, mac, cipher_text, len, secret_key);

    char ext[] = ".dec";
    new_path = malloc(strlen(path) + sizeof(ext));
    memcpy(new_path, path, strlen(path));
    memcpy(new_path + strlen(path), ext, sizeof(ext));

    fp2 = fopen(new_path, "w");
    if (!fp2) {
        printf("fopen failed\n");
        err = 1;
        goto cleanup;
    }
    fwrite(plain_text, sizeof(*plain_text), len, fp2);

cleanup:
    if (fp)          fclose(fp);
    if (fp2)         fclose(fp2);
    if (modulo)      free(modulo);
    if (secret_key)  free(secret_key);
    if (new_path)    free(new_path);
    if (cipher_text) free(cipher_text);
    if (plain_text)  free(plain_text);

    if (err) exit(1);
}

int main(int argc, char **argv)
{
    if (argc < 2) {
        print_usage();
        return 0;
    }

    if (strcmp(argv[1], "lock") == 0) {
        if (argc != 4) {
            print_usage();
            return 0;
        }

        uint64_t squarings = strtoll(argv[2], NULL, 10);
        if (!squarings) {
            print_usage();
            return 0;
        }

        char *path = argv[3];
        printf("Locking %s\n", path);
        lock_file(path, squarings);
        printf("Time-locked message written to %s.enc\n", path);

    } else if (strcmp(argv[1], "unlock") == 0) {
        if (argc != 3) {
            print_usage();
            return 0;
        }

        char *path = argv[2];
        printf("Unlocking %s\n", path);
        unlock_file(path);
        printf("Unlocked message written to %s.dec\n", path);

    } else {
        print_usage();
    }

    return 0;
}
