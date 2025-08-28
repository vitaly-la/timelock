#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#include "crypto.h"
#include "gen_puzzle.h"

static void print_usage(void)
{
    printf("Usage: timelock lock <squarings>|<seconds>s <filename>\n"
           "       timelock unlock <filename>\n");
}

static void lock_file(const char *path, uint64_t squarings)
{
    int fd = open(path, O_RDONLY);

    struct stat sb;
    fstat(fd, &sb);
    size_t len = (size_t)sb.st_size;

    char *addr = mmap(NULL, len, PROT_READ, MAP_PRIVATE, fd, 0);

    char *secret_key, *modulo;
    gen_puzzle(&secret_key, &modulo, squarings);
    printf("Generated modulo string: %s\n", modulo);

    encrypt(addr, len, secret_key);

    char ext[] = ".enc";
    char *new_path = malloc(strlen(path) + sizeof(ext));
    memcpy(new_path, path, strlen(path));
    memcpy(new_path + strlen(path), ext, sizeof(ext));

    FILE *fp = fopen(new_path, "w");
    fwrite(addr, sizeof(*addr), len, fp);

    fclose(fp);
    free(new_path);
    free(secret_key);
    free(modulo);
    munmap(addr, len);
    close(fd);
}

static void unlock_file(const char *path)
{
    int fd = open(path, O_RDONLY);

    struct stat sb;
    fstat(fd, &sb);
    size_t len = (size_t)sb.st_size;

    char *addr = mmap(NULL, len, PROT_READ, MAP_PRIVATE, fd, 0);

    char ext[] = ".dec";
    char *new_path = malloc(strlen(path) + sizeof(ext));
    memcpy(new_path, path, strlen(path));
    memcpy(new_path + strlen(path), ext, sizeof(ext));

    FILE *fp = fopen(new_path, "w");
    fwrite(addr, sizeof(*addr), len, fp);

    fclose(fp);
    free(new_path);
    munmap(addr, len);
    close(fd);
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

        uint64_t squarings = (uint64_t)strtoll(argv[2], NULL, 10);
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
