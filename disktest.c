#include <stdio.h>
#include <fcntl.h>
#include <stdint.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

#define BLKSZ 4096
#define CHUNK (BLKSZ / sizeof(uint64_t))

uint64_t block[CHUNK];
uint64_t block2[CHUNK];

int main(int argc, char **argv) {
    if (argc != 2) {
        printf("Usage: %s /dev/sd???\n", argv[0]);
        return 10;
    }

    int mode = 0;
    if (!strcmp(argv[0], "dtest")) {
        mode = 1;
    }
    if (!strcmp(argv[0], "./dtest")) {
        mode = 1;
    }

    int fd = open(argv[1], O_RDWR | O_SYNC);
    if (fd <= 0) {
        printf("Unable to open %s\n", argv[1]);
        return 10;
    }

    uint64_t counter = 0;

    time_t t = time(NULL);

    int mbb = -1;

    while (1) {
        int i;

        if (mode == 0) {
            for (i = 0; i < CHUNK; i++) {
                block[i] = counter++;
            }

            if (write(fd, block, BLKSZ) != BLKSZ) {
                printf("\rWritten %d MiB\n", (counter * sizeof(uint64_t)) / 1048576);
                fflush(stdout);
                close(fd);
                return 0;
            }

            off_t here = lseek(fd, 0, SEEK_CUR);

            lseek(fd, here-BLKSZ, SEEK_SET);
            if (read(fd, block2, BLKSZ) != BLKSZ) {
                printf("\nWrite error at %lu\n", (counter * sizeof(uint64_t)));
                close(fd);
                return 0;
            }

            for (i = 0; i < CHUNK; i++) {
                if (block2[i] != block[i]) {
                    printf("\nVerify error at %lu (%lu != %lu)\n", (counter * sizeof(uint64_t)), block2[i], block[i]);
                    close(fd);
                    return 0;
                }
            }
        
        } else {

            if (read(fd, block, BLKSZ) != BLKSZ) {
                printf("\rRead %d MiB\n", (counter * sizeof(uint64_t)) / 1048576);
                fflush(stdout);
                close(fd);
                return 0;
            }

            for (i = 0; i < CHUNK; i++) {
                if (block[i] != counter) {
                    printf("\nRead error at %lu (%lu != %lu)\n", counter * sizeof(uint64_t), block[i], counter);
                    close(fd);
                    return 0;
                }

                counter++;
            }
    
        }

        int mb = (counter * sizeof(uint64_t)) / 1048576;
        if (mb != mbb) {
            mbb = mb;
            printf("\r%s %d MiB", mode == 0 ? "Written" : "Read", mb);
            fflush(stdout);
        }
    }
}
