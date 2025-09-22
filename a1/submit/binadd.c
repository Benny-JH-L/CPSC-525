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


/// Benny Liang | UCID: 30192142 | Assignment 1 - Coding Part

#include "binadd.h"
#include "errno.h"
#include "fcntl.h"
#include "stdint.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>

/// @brief Valid n-bit long file sizes
enum AcceptedBytes
{
    BITS8 = 1,
    BITS16 = 2,
    BITS32 = 4,
};

int closeFile(int fd);

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


    int fd = open(fname, O_RDWR, 0600);  // open the file with read/write perms, first read(2).
    if (fd == -1)   // an error occured
    {
        if (errno != ENOENT)    // "Error No Entry/Enitity" check, if the `errno` is that then we create the file.
            return BA_EOPEN;
        
        // printf("open() failed because:\n\terrno = %d (%s)\n", errno, strerror(errno));   // debug

        fd = open(fname, O_CREAT | O_RDWR | O_EXCL, 0600);    // create file with perms "0600" (-rw-------), second read(2)
        
        if (fd == -1)   // check if the file was created successfully
        {
            // printf("open() | File creation | failed because:\n\terrno = %d (%s)\n", errno, strerror(errno));  // debug
            return BA_ECREATE;
        }
        
        // write 32 bit int `0` to `fd`, and check if write was successful
        uint32_t val = 0;     
        ssize_t wrote = write(fd, &val, sizeof(val));
        // ssize_t wrote = write(fd, &val, 1);  // debug
        if (wrote == -1 || wrote != sizeof(val))
        {
            // printf("Could not write `0`:\n\terrno = %d (%s)\n", errno, strerror(errno));  // debug
            closeFile(fd);
            return BA_EWRITE;
        }
        
        // printf("wrote %ld bytes.\n", wrote);   // debug

        // close the file
        if (closeFile(fd) != 0)
            return BA_ECLOSE;
        
        return BA_SUCCESS;
    }

    // Get file size
    ssize_t fileSize = lseek(fd, 0, SEEK_END);  // first lseek(2)
    if (fileSize == -1) // check for error
    {
        // printf("lseek() going to END failed:\n\terrno = %d (%s)\n", errno, strerror(errno));  // debug
        closeFile(fd);
        return BA_ESEEK;
    }
    // printf("file size is: %ld bytes\n", fileSize);  // debug
    
    // rewind lseek to start of file 
    if (lseek(fd, 0, SEEK_SET) == -1) // check for error, second lseek(2)
    {
        // printf("lseek() going to FRONT failed:\n\terrno = %d (%s)\n", errno, strerror(errno));  // debug
        closeFile(fd);
        return BA_ESEEK;
    }

    // check for valid size
    switch (fileSize)
    {
        case BITS8:
            break;
        case BITS16:
            break;
        case BITS32:
            break;
        default:    // not a valid size
            closeFile(fd);  // close the file
            return BA_ESIZE;
    }

    // load contents
    uint32_t count = 0;    // contents should be an unsigned 
    ssize_t readBytes = read(fd, &count, fileSize);

    // debug
    // printf("sizeof(fileSize): %ld\n", sizeof(fileSize));
    // printf("sizeof(count): %ld\n", sizeof(count));

    if (readBytes == -1 || readBytes != fileSize)    // check for error
    {
        // printf("read() failed:\n\terrno = %d (%s)\n", errno, strerror(errno));  // debug
        closeFile(fd);
        return BA_EREAD;
    }
    // printf("Bytes read: %lu\nCount num read: %u\n", readBytes, count);  // debug
    
    // check for overflow
    int overFlowDetected = 0;   // true/false variable, set to false initially 
    if (fileSize == BITS8 && count == UINT8_MAX)
        overFlowDetected = 1;
    else if (fileSize == BITS16 && count == UINT16_MAX)
        overFlowDetected = 1;
    else if (fileSize == BITS32 && count == UINT32_MAX)
        overFlowDetected = 1;

    if (overFlowDetected)
    {
        // printf("overflow detected!\n");   // debug
        closeFile(fd);
        return BA_EMAX;
    }

    count++;    // increment the count by 1
    // count = UINT32_MAX;  // for debugging

    if (lseek(fd, 0, SEEK_SET) == -1)   // rewind lseek to start of file, third lseek(2)
    {
        // printf("lseek() going to FRONT failed:\n\terrno = %d (%s)\n", errno, strerror(errno));  // debug
        closeFile(fd);
        return BA_ESEEK;
    }

    // debug
    // printf("fileSize before write: %lu\n", fileSize);
    // printf("writting new count: %u\n", count);

    // Write `count`
    ssize_t bytesWritten = write(fd, &count, fileSize);
    // printf("Wrote: %lu bytes\n", bytesWritten);  // debug
    if (bytesWritten == -1 || bytesWritten < fileSize)
    {
        // printf("Could not write `count` number:\n\terrno = %d (%s)\n", errno, strerror(errno));  // debug
        closeFile(fd);
        return BA_EWRITE;
    }

    // Close file
    if (closeFile(fd) != 0)
        return BA_ECLOSE;

    return BA_SUCCESS;
}

/// @brief Return's 0 on success. Returns `BA_ECLOSE` otherwise.
/// @param fd file descriptor.
/// @return an int.
int closeFile(int fd)
{
    if (fd <= -1)
        return BA_ECLOSE;
    
    else if (close(fd) == -1)
    {
        // printf("close() failed because:\n\terrno = %d (%s)\n", errno, strerror(errno));  // debug
        return BA_ECLOSE;
    }

    return 0;
}
