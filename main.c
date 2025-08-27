#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

void print_usage()
{
    printf("Usage: timelock lock <squarings>|<seconds>s <filename>\n"
           "       timelock unlock <filename>\n");
}

void gen_puzzle(unsigned long long squarings)
{
}

void lock_file(const char *path, unsigned long long squarings)
{
    int fd = open(path, O_RDONLY);

    struct stat sb;
    fstat(fd, &sb);
    size_t len = sb.st_size;

    char *addr = mmap(NULL, len, PROT_READ, MAP_PRIVATE, fd, 0);

    ;

    munmap(addr, len);
    close(fd);
}

void unlock_file(const char *path)
{
}

int main(int argc, char **argv)
{
    if (argc < 2) {
        print_usage();
        exit(0);
    }

    if (strcmp(argv[1], "lock") == 0) {
        if (argc != 4) {
            print_usage();
            exit(0);
        }

        unsigned long long squarings = strtoll(argv[2], NULL, 10);
        char *path = argv[3];
        printf("Locking %s\n", path);
        lock_file(path, squarings);
        printf("Time-locked message written to %s.enc\n", path);

    } else if (strcmp(argv[1], "unlock") == 0) {
        if (argc != 3) {
            print_usage();
            exit(0);
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
