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

#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <error.h>
#include <vector>
#include <cmath>

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
const char * guess3(const char * exepath)
{
    const int DEBUG = getenv("DEBUG") != nullptr;
    static char secret[1024];

    vector<char> validPasswordChars;    // characters will be sorted with their ascii from lowest to greatest

    // add digits 0 to 9
    for (char digit = '0'; digit <= '9'; digit++)
        validPasswordChars.emplace_back(digit);

    // get the letters from 'a' to 'z' and put them into the vector<char>
    // get the ascii of 'a' and continuly add 1 to get the rest of the letters
    for (char asciiChar = 'a'; asciiChar <= 'z'; asciiChar++)
        validPasswordChars.emplace_back(asciiChar);
    
    // debug
    if (DEBUG)
    {
        cout << "Printing valid password chars (size: " << validPasswordChars.size() << "):\n\t";
        for (char c : validPasswordChars)
            cout << c << ", ";    
        cout << endl;

        cout << "Printing valid password char ASCII's:\n\t";
        for (int c : validPasswordChars)
            cout << c << ", ";  
        cout << "\n" << endl;
    }

    bool foundPass = false;     // if we've found the hardcoded password
    for (int i = 0; i <= MAX_PASS_LENGTH && !foundPass; i++)
    // for (int i = 0; i <= 4 && !foundPass; i++)   // debug
    {
        // for debugging
        int8_t prevReturnOf_strcmp = 0;
        char prevCharGuess = validPasswordChars[0];

        // call the vulnerable program until we find the correct hardcoded character
        // will be using a sort of binary search to find the character
        int low = 0;                                        // min index when searching though `validPasswordChars` vector
        int high = validPasswordChars.size() - 1;           // max index when searching though `validPasswordChars` vector
        vector<char> vecGuessesSoFar;       // stores the characters guessed at index `i` of the hardcoded password
        vector<int8_t> vecStrcmpReturns;    // stores what strcmp() returns for a given guess at index
        while (low <= high && !foundPass)
        // for (int j = 0; j <= 6 && !foundPass; j++)   // debug
        {
            int mid = low + floor((high - low) / 2);
            if (DEBUG)
            {
                cout << "\nlow = " << low << " | mid = " << mid << " | high = " << high;
                cout << "\nvalidPasswordChars["<<mid<<"] ='" << validPasswordChars[mid] << "' | ascii = " << (int)validPasswordChars[mid];
                cout << "\nprev char guess ='" << prevCharGuess << "' | ascii = " << (int)prevCharGuess << endl;
            }

            char charGuess = validPasswordChars[mid];
            vecGuessesSoFar.emplace_back(charGuess);   // add the character we guessed
            secret[i] = charGuess;                  // try character
            setenv("SECRET", secret, 1);            // set the environment var

            int statusSys = system(exepath);
            int exitCode = WEXITSTATUS(statusSys);
            int8_t returnOf_strcmp = (int8_t)exitCode;  // the return value `res` of strcmp() in secret3.c
            vecStrcmpReturns.emplace_back(returnOf_strcmp);
            if (DEBUG)
            {
                printf("Trying secret '%s'\n", secret);
                printf("\nreturn val of strcmp() inside `%s`: '%d'\n", exepath, returnOf_strcmp);
                // printf("returned status of system(%s): '%d'\n", exepath, statusSys);
                // printf("exit code status of `%s`: '%d'\n", exepath, exitCode);
            }

            // update `low`, or `high` depending on strcmp() return val.
            if (returnOf_strcmp > 0)
            {
                // hardcoded character is "less than" our character guess
                low = mid + 1;
                DEBUG && cout << "setting low = " << low << endl;
            }
            else if (returnOf_strcmp < 0)
            {
                // hardcoded character is "greater than" our character guess
                high = mid - 1;
                DEBUG && cout << "setting high = " << high << endl;
            }
            else if (returnOf_strcmp == 0)
            {
                // password constructed in `secret` var is the same as the hardcoded password!
                foundPass = true;
                DEBUG && cout << "[FOUND] found the hardcoded password: `" << secret << "`" << endl;
                return secret;      // return the hardcoded password found
            }

            if (DEBUG)
            {
                cout << "prevReturnOf_strcmp = '"<< (int)prevReturnOf_strcmp <<"'" << endl;
                cout << "returnOf_strcmp = '"<< (int)returnOf_strcmp <<"'" << endl;
            }

            // debugging
            prevCharGuess = validPasswordChars[mid];
            prevReturnOf_strcmp = returnOf_strcmp;
        }

        if (DEBUG)
        {
            cout << "printing guesses done: ";
            for (char c : vecGuessesSoFar)
            {
                cout << c << ", ";
            }
            cout << endl;
        }

        // go though guesses in reverse order for secret character indexed at `i` until 
        // the return value of strcmp() is 1
        int j = vecGuessesSoFar.size() - 1;
        for (auto it = vecStrcmpReturns.end() - 1; it >= vecStrcmpReturns.begin() ; it--, j--)
        {
            // if the return value of strcmp() is -1 in `vecStrcmpReturns[j]`, this character `guessesSoFar[j]` is not the hardcoded letter
            if (*it == 1)
            {              
                // `guessesSoFar[j]` character is the hardcoded character in `secret[i]`
                DEBUG && cout << "correct letter should be: " << vecGuessesSoFar[j] << endl;
                secret[i] = vecGuessesSoFar[j];
                break;
            }
        }

        // OLD
        // for (int j = vecGuessesSoFar.size() - 1; j >= 0; j--)
        // {
        //     secret[i] = vecGuessesSoFar[j];    // try character
        //     setenv("SECRET", secret, 1);    // set the environment variable
        //     DEBUG && printf("(going though guesses so far) Trying secret '%s'\n", secret);

        //     int statusSys = system(exepath);
        //     int exitCode = WEXITSTATUS(statusSys);
        //     int8_t returnOf_strcmp = (int8_t)exitCode;      // the return value `res` of strcmp() in secret3.c
        //     DEBUG && cout << "returnOf_strcmp = '"<< (int)returnOf_strcmp <<"'" << endl;

        //     // if the return value of strcmp() is -1, this character `guessesSoFar[j]` is not the hardcoded letter
        //     if (returnOf_strcmp == 1)
        //     {
        //         // `guessesSoFar[j]` character is the hardcoded character in `secret[i]`
        //         DEBUG && cout << "correct letter should be: " << vecGuessesSoFar[j] << endl;
        //         break;
        //     }
        // }
        
    }

    return NULL;    // hardcoded password could not be found
}
