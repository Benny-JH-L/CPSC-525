/// =========================================================================
/// Copyright (C) 2025 Pavol Federl (pfederl@ucalgary.ca)
/// All Rights Reserved. Do not distribute this file.
/// =========================================================================
///
/// DO NOT MODIFY this file and DO NOT SUBMIT it for grading

/// -------------------------------------------------------------------------
/// This program has a hardcoded password, which is set at compile time
/// using -DSTORED_SECRET=\"somesecret\".
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

#ifndef STORED_SECRET
#warning STORED_SECRET not defined, using default value "000abc"
#define STORED_SECRET "000abc"
#endif

#include <error.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// convert string to integer
int64_t str2long(const char * s)
{
    int64_t res;
    strncpy((char *) &res, s, sizeof(int32_t) - 1);
    // printf("string2long<%s> = <%ld>\n", s, res);    // mine
    return res;
}

// compare strings super-fast using a single integer comparison
int compare_secrets_1(const char * s1, const char * s2)
{

    // int res = str2long(s1) == str2long(s2) ? 0 : 1; // mine
    // printf("str2long(%s) == str2long(%s) | (%d)\n", s1, s2, res); // mine
        // remove the above code to get shorter `res` values when str2long() is called

    return (str2long(s1) == str2long(s2)) ? 0 : 1;
    // return res; // mine 
}

int main(int argc, char ** argv)
{
    // read the guess from environment variable "SECRET"
    char * guess = getenv("SECRET");
    if (guess == NULL) {
        error(0, 0, "no SECRET given\n");
        abort();
    }
    // compare the guess to the stored secret using totally not
    // suspicious algorithm
    int res = compare_secrets_1(STORED_SECRET, guess);
    return res;
}
