/// =========================================================================
/// =========================================================================
/// Copyright (C) 2025 Pavol Federl (pfederl@ucalgary.ca)
/// All Rights Reserved. Do not distribute this file.
/// =========================================================================
///
/// DO NOT MODIFY this file and DO NOT SUBMIT it for grading
///

/// -------------------------------------------------------------------------
/// This program has a hardcoded password, which is set at compile time
/// using -DSTORED_SECRET=\"somesecret\".
///
/// It also has a hardcoded delay, set at compile time using -DDELAY=0.1.
///
/// The program expects a password guess to be supplied via environment
/// variable "SECRET".
///
/// The program exits with status=0 if the password was correctly guessed.
/// The program exits with non-zero exit status otherwise.
///
/// For example, to check if '0123' is the stored password, run
/// the following command:
///
/// $ SECRET=0123 ./guess ; echo $?
///
/// The above will print 0 if the guess was successful,
/// and something other than 0 if unsuccessful.
/// -------------------------------------------------------------------------

/// made some edits to show progress of wheb guess3.cpp calls this file.


#ifndef STORED_SECRET
#warning STORED_SECRET not defined, using default value "000abc"
#define STORED_SECRET "000abc"
#endif

#ifndef DELAY
#warning DELAY not defined, using default value 0.1
#define DELAY 0.1
#endif

#include <error.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>

double now()
{
    const double billionth = 1e-9;
    struct timespec t;
    int res = clock_gettime(CLOCK_MONOTONIC, &t);
    if (res) {
        error(0, errno, "clock_gettime() failed");
        abort();
    }
    return t.tv_sec + billionth * t.tv_nsec;
}

// sleep for amount seconds
void dsleep(double amount)
{
    if (amount <= 0) return;
    double end = now() + amount;
    while (1) {
        double remain = end - now();
        if (remain <= 0) return;
        struct timespec ts;
        ts.tv_sec = remain / 2;
        ts.tv_nsec = (remain / 2 - ts.tv_sec) * 1000000000;
        nanosleep(&ts, NULL);
    }
}

// compare strings using strcmp with slightly modified
// return values
int compare_secrets_3(const char * s1, const char * s2)
{
    // to slow down brute-force attacks, we artifically delay
    // returning the result
    dsleep(DELAY);
    int res = strcmp(s1, s2);
    printf("\n[secret2.c] result of strcmp(%s, %s) = %d\n", s1, s2, res);   // mine
    if (res < 0) {
        return -1;
    } else if (res > 0) {
        return 1;
    } else {
        return 0;
    }
}

int main(int argc, char ** argv)
{
    char * guess = getenv("SECRET");
    if (guess == NULL) {
        error(0, 0, "no SECRET given\n");
        abort();
    }
    int res = compare_secrets_3(STORED_SECRET, guess);
    return res;
}
