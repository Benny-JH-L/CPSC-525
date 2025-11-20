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

#include <errno.h>
#include <error.h>
#include <stdio.h>
#include <stdlib.h>

#include <string>
#include <vector>

// testing
#include <unordered_map>
#include <string.h>
#include <iostream>

using namespace std;

int64_t str2long(const char * s);
static unordered_map<int64_t, vector<string>> long2StringMap;

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
const char * guess1(const char * exepath)
{
    // the function str2long() only copies and returns the first 3 bytes (32 bits - 1byte = 3 bytes) of the input string,
    // so we only need to test passwords of length 3 over the valid characters for passwords ('a' to 'z' and 0-9).
    // Which will be 36^3 = 46 656 passwords to test!

    const int DEBUG = getenv("DEBUG") != nullptr;

    vector<char> validPasswordChars;

    // get the letters from 'a' to 'z' and put them into the vector<char>
    // get the ascii of 'a' and continuly add 1 to get the rest of the letters
    for (char asciiChar = 'a'; asciiChar <= 'z'; asciiChar++)
        validPasswordChars.emplace_back(asciiChar);
    
    // add digits 0 to 9
    for (char digit = '0'; digit <= '9'; digit++)
        validPasswordChars.emplace_back(digit);
    
    // debug
    if (DEBUG)
    {
        cout << "Printing valid password chars (size: " << validPasswordChars.size() << "):\n\t";
        for (char c : validPasswordChars)
            cout << c << ", ";    
        cout << endl;
    }

    static char secret[1024];
    
    // 1st letter
    for (char& c1 : validPasswordChars)
    {
        // 2nd letter
        for (char& c2 : validPasswordChars)
        {
            // 3rd letter
            for (char& c3 : validPasswordChars)
            {
                // create the secret
                secret[0] = c1;
                secret[1] = c2;
                secret[2] = c3;

                setenv("SECRET", secret, 1);
                DEBUG && printf("Trying secret '%s'\n", secret);
                int status = system(exepath);   // run the vulnerable executable
                if (! WIFEXITED(status) || WEXITSTATUS(status) == 127) 
                {
                    if (DEBUG) error(0, errno, "system(%s) failed", exepath);
                    return NULL;
                }
                // check if executable reported success
                if (WEXITSTATUS(status) == 0) 
                {
                    // success !!!
                    DEBUG && printf("success\n");
                    return secret;
                }
            }
        }
    }
    return NULL;

    // OLD CODE:
    std::unordered_map<int64_t, int64_t> map;

    int offset = 1, offset2 = 2;

    for (int i = -1 + offset; i < 11 + offset + offset2; i++) 
    {
        sprintf(secret, "%s", std::to_string(i).c_str());
        // sprintf(secret, "%03d", i);  // original
        setenv("SECRET", secret, 1);

        map[str2long(secret)]++;

        DEBUG && printf("Trying secret '%s'\n", secret);
        int status = system(exepath);
        // if system() fails, report error on stderr and return NULL
        if (! WIFEXITED(status) || WEXITSTATUS(status) == 127) 
        {
            if (DEBUG) error(0, errno, "system(%s) failed", exepath);
            return NULL;
        }
        // check if executable reported success
        if (WEXITSTATUS(status) == 0) 
        {
            // success !!!
            DEBUG && printf("success\n");
            return secret;
        }
    }

    for (const auto pair : map)
    {
        printf("key (long val) = %ld | val (number of occurances) = %ld\n", pair.first, pair.second);
        vector<string> strVec = long2StringMap[pair.first];
        printf("key(s) before str-long conversions was: \n\t");
        for (auto s : strVec)
            printf("%s, ", s.c_str());
        printf("\n");
    }
    // none of the attempts succeeded
    return NULL;
}

int64_t str2long(const char * s)
{
    int64_t res;
    strncpy((char *) &res, s, sizeof(int32_t) - 1);
    long2StringMap[res].push_back(string(s));
    printf("string2long<%s> = <%ld>\n", s, res);  
    return res;
}


// const char * guess1(const char * exepath)
// {
//     // the following implementation is not very good
//     // - it only checks secrects of exactly 3 digits, i.e. 000-999
//     //   But a secret could be any length, and consist of digits and
//     //   lower case letters as well, i.e. 0-zzzzzzzzzzz...
//     // - it uses system(3), which is slower than necessary.
//     //   This is because system() invokes the given command
//     //   by starting a /bin/sh shell first.
//     // - it displays debugging output, which your solution
//     //   should not (the debug output below can be controlled by
//     //   changing the DEBUG variable from 0 to 1)
//     const int DEBUG = getenv("DEBUG") != nullptr;
//     static char secret[1024];
//     for (int i = 0; i < 1000; i++) {
//         sprintf(secret, "%03d", i);
//         setenv("SECRET", secret, 1);
//         DEBUG && printf("Trying secret '%s'\n", secret);
//         int status = system(exepath);
//         // if system() fails, report error on stderr and return NULL
//         if (! WIFEXITED(status) || WEXITSTATUS(status) == 127) {
//             if (DEBUG) error(0, errno, "system(%s) failed", exepath);
//             return NULL;
//         }
//         // check if executable reported success
//         if (WEXITSTATUS(status) == 0) {
//             // success !!!
//             DEBUG && printf("success\n");
//             return secret;
//         }
//     }
//     // none of the attempts succeeded
//     return NULL;
// }
