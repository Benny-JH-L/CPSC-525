/// (c) 2025, Pavol Federl, pfederl@ucalgary.ca
/// Do not distribute this file.
/// -------------------------------------------------------------------------------------
/// this is the only file you need to edit
/// -------------------------------------------------------------------------------------
///
/// You are allowed limited collaboration on the assignment as discussed in the class.
/// You can discuss the assignment and even design the solution with one other classmate,
/// but you must then write your code by yourself. While some similarity to your collaborator
/// will be tolerated, you may not submit identical code.
///
/// if you collaborated on this assignment with another student,
/// list their name here: ________________________________
/// 

// use command: "hexdump <filename>" or "hexdump -C <filename>"
// or try using: "xxd <filename>""

#include "binadd.h"
#include "errno.h"
#include "fcntl.h"
#include "stdint.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>

// 1 byte = 8 bits
// off_t const bits8 = 1;
// const long int bits8 = 1;
// long int const bits16 = 2;
// long int const bits32 = 4; 

enum AcceptedBytes
{
    BIT8 = 1,
    BITS16 = 2,
    BITS32 = 4,
};

int closeFile(int fd);
int testIncrementFile(const char * fname);

int binadd(const char * fname)
{
    /*
    - use open() to open existing file
        - if unsuccessful because file does not exist
            - try to create a file with 4-byte counter set to 0
              using open(), write() and close()
            - return error if appropriate, otherwise return success
        - if unsuccessful for any other other reason, return error
    -use lseek() to determine file size
        - return error if lseek fails or file has wrong size
    - use lseek() to rewind then read() to get counter value from the file
        - return appropriate error if unsuccessful
    - increment counter
    - use lseek() to rewind then write() to save the counter to the file
        - return appropriate error if unsuccessful
    - close() the file
        - return appropriate error if unsuccessful
    - return success
    */

    // if (1)
    // {
    //     printf("ewiugfbewigvbeiwweib\n");
    //     testIncrementFile(fname);
    //     return -1;
    // }

    int fd = open(fname, O_RDWR | O_EXCL, 0777);  // open the file with read/write perms
    if (fd == -1)
    {
        // printf("open() failed because:\n"
        //        "    errno = %d (%s)\n", errno, strerror(errno)); 
        
        fd = open(fname, O_CREAT | O_RDWR | O_EXCL, 0600);    // create file with perms "0600" (-rw-------)
        // fd = open(fname, O_CREAT | O_RDWR | O_EXCL, 0777);    // ONLY FOR DEBUGGING
        
        if (fd == -1)   // check if the file was created successfully
        {
            // printf("open() | File creation | failed because:\n"
            //     "   errno = %d (%s)\n", errno, strerror(errno));
            return BA_ECREATE;
        }
        
        uint32_t val = 0;     
        int64_t wrote = write(fd, &val, sizeof(val));
        // int wrote = write(fd, &val, 3);  // debug
        // write 32 bit int `0` to fd, and check if write was successful
        if (wrote == -1)
        {
            // printf("Could not write `0`:\n"
            //     "   errno = %d (%s)\n", errno, strerror(errno));
            closeFile(fd);
            return BA_EWRITE;
        }
        // else
        //     printf("wrote %ld bytes.\n", wrote);

        int code = closeFile(fd);
        if (code != 0)
        {
            // printf("Hey smthing went wrong\n");
            return code;
        }
        // else 
        //     printf("good closed:)\n");


        return errno;
    }


    // Get file size
    ssize_t fileSize = lseek(fd, 0, SEEK_END);
    if (fileSize == -1) // check for error
    {
        // printf("lseek() going to END failed:\n"
        //     "   errno = %d (%s)\n", errno, strerror(errno));
        return BA_ESEEK;
    }
    // printf("file size is: %ld bytes\n", fileSize);
    
    // enum AcceptedBytes {BIT8 = BIT8, BITS16 = BITS16, BITS32 = BITS32};

    // check for valid size
    switch (fileSize)
    {
        case BIT8:
            break;
        case BITS16:
            break;
        case BITS32:
            break;
        default:    // not a valid size
            return BA_ESIZE;
    }

    // rewind lseek to start of file 
    if (lseek(fd, 0, SEEK_SET) == -1) // check for error
    {
        // printf("lseek() going to FRONT failed:\n"
            // "   errno = %d (%s)\n", errno, strerror(errno));
        return BA_ESEEK;
    }
    
    // load contents
    uint32_t count;    // contents should be unsigned
    // ssize_t readBytes = read(fd, &count, fileSize);  // 
    ssize_t readBytes = read(fd, &count, sizeof(count));

    if (readBytes == -1)    // check for error
    {
        // printf("read() failed:\n"
            // "   errno = %d (%s)\n", errno, strerror(errno));
        return BA_EREAD;
    }
    // printf("Bytes read: %lu\nCount num read: %u\n", readBytes, count);

    // check for overflow
    switch (fileSize)
    {
        case BIT8:
            if (count == UINT8_MAX)
                return BA_EMAX;
            break;
        case BITS16:
            if (count == UINT16_MAX)
                return BA_EMAX;
            break;
        case BITS32:
            if (count == UINT32_MAX)
                return BA_EMAX;
            break;
        // default:
        //     printf("file size (again lol): %lu\n", fileSize);
    }

    count++;
    // count = UINT32_MAX;  // for debugging
    fileSize = lseek(fd, 0, SEEK_SET);   // rewind lseek to start of file 
    // printf("writting new count: %u\n", count);

    // check for write error
    if (write(fd, &count, sizeof(count)) == -1)
    {
        // printf("Could not write count numer:\n"
            // "   errno = %d (%s)\n", errno, strerror(errno));
        closeFile(fd);
        return BA_EWRITE;
    }

    closeFile(fd);

    return BA_SUCCESS;
}

/// @brief Return's 0 on success. Returns `BA_ECLOSE` otherwise.
/// @param fd file descriptor.
/// @return an int.
int closeFile(int fd)
{
    if (close(fd) == -1)
    {
        // printf("close() failed because:\n"
            // "   errno = %d (%s)\n", errno, strerror(errno));
        return BA_ECLOSE;
    }
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