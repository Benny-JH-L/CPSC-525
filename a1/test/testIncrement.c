/* sample C code for incrementing a 32-bit integer stored in a file*/
#include <assert.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
int main(int argc, char ** argv) 
{
    if (argc != 2) 
    {
        fprintf(stderr, "Need 1 argument!\n");
        return -1;
    }
    testIncrementFile(argv[1]);
    return 0;
}

int testIncrementFile(const char * fname)
{
    int fd = open(fname, O_CREAT | O_RDWR, 0777);
    printf("ewf\n");
    uint32_t num;
    ssize_t len = read(fd, &num, 4);
    printf("num read: %u\nbytes read: %lu\n", num, len);
    (void) len; /* disables compiler warnings */
    off_t res = lseek(fd, 0, SEEK_SET);
    (void) res;
    num++;
    printf("writting num: %u\n", num);
    len = write(fd, &num, 4);
    (void) len;
    int res2 = close(fd);
    if (res2 != 0) { return 1; }
    return 0;
}
