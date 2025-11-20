/// =========================================================================
/// Copyright (C) 2025 Pavol Federl (pfederl@ucalgary.ca)
/// All Rights Reserved. Do not distribute this file.
/// =========================================================================

// You need to implement the guess<n>() function below
// and submit this file for grading.
//
// Feel free to change anything in this file, execpt the signature
// of the guess<n>() function.

#include <stdlib.h>

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
const char * guess3(const char * exepath)
{
    // needs to be implemented
    return NULL;
}
