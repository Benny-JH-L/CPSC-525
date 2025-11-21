/// =========================================================================
/// Copyright (C) 2025 Pavol Federl (pfederl@ucalgary.ca)
/// All Rights Reserved. Do not distribute this file.
/// =========================================================================

// You need to implement the guess<n>() function below
// and submit this file for grading.
//
// Feel free to change anything in this file, execpt the signature
// of the guess<n>() function.

// Benny Liang | UCID: 30192142 | Assignment 4

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <error.h>

using namespace std;

const int MAX_PASS_LENGTH = 64;

// The guess<N>() function tries to guess a hardcoded secret from a compiled
// executable, whos path is given as `exepath` parameter.
// The input executable is assumed to behave as follows:
//  - it will read the secret from environment variable "SECRET"
//  - it will return exit code 0 if the secret was guessed correctly
//  - it will return non-zero exit code otherwise
//  - the executable will be a compiled from secret<N>.c
//
// Return value:
//  - pointer to statically allocated string containing the guessed secret,
//    null-terminated
//  - NULL if the secret could not be guessed
//
// Make sure your function does not produce any output to stdout or stderr.
const char * guess2(const char * exepath)
{
    const int DEBUG = getenv("DEBUG") != nullptr;
    static char secret[1024];
    
    const int ASCII_z = 'z';
    int8_t exitCodeOf_strcmp = 1;
    // for (int index = 0; index <= MAX_PASS_LENGTH && exitCodeOf_strcmp != ASCII_z; index++)
    for (int index = 0; index <= MAX_PASS_LENGTH; index++)
    {
        secret[index] = 'z';

        setenv("SECRET", secret, 1);
        DEBUG && printf("Trying secret '%s'\n", secret);

        int statusSys = system(exepath);
        DEBUG && printf("\nreturned status of system(%s): '%d'\n", exepath, statusSys);
        int exitCode = WEXITSTATUS(statusSys);
        exitCodeOf_strcmp = (int8_t)exitCode;
        DEBUG && printf("exit code status of `%s`: '%d'\n", exepath, exitCode);
        DEBUG && printf("strcmp() result inside `%s`: '%d'\n", exepath, exitCodeOf_strcmp);
    
        if (! WIFEXITED(statusSys) || WEXITSTATUS(statusSys) == 127) 
        {
            if (DEBUG) error(0, errno, "system(%s) failed", exepath);
            return NULL;
        }
        // check if executable reported success
        if (WEXITSTATUS(statusSys) == 0) 
        {
            // success !!!
            DEBUG && printf("success\n");
            return secret;
        }

        if (exitCodeOf_strcmp == exitCode)
        {
            // case where the hardcoded char is the char we are using to guess with
            DEBUG && cout << "found char 'z' | strcmp() return = " << (int)exitCodeOf_strcmp << " | return code of '" << exepath << "' = " << exitCode << endl;
            secret[index] = 'z'; // update the secret to the hardcoded char we found 
            continue;
        }
        // found the terminating '\0' character, ie we tried a password of length 1 greater than the hardcoded password
        else if (exitCodeOf_strcmp == -ASCII_z)
        {
            secret[index] = (char)NULL;   // remove the last character we added
            return secret;
        }

        // find the hardcoded char from return value `exitCodeOf_strcmp`
        char letter = (char)(ASCII_z + exitCodeOf_strcmp);  // from ASCII 'z' the hardcoded character will be `exitCodeOf_strcmp` ASCII away (will be negative, even if its a digit)
        DEBUG && printf("hardcoded letter found: '%c'\n", letter);
        secret[index] = letter; // update the secret to the hardcoded char we found
    }

    return NULL;
}
