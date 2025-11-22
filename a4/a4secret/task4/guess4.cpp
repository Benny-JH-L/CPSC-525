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
#include <chrono>
#include <vector>

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
const char * guess4(const char * exepath)
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

    // `warmup`
    DEBUG && cout << "----warming up...\n";
    secret[0] = 'z';        // set some char for warmup
    for (int i = 0; i < 2; i++)
    {        
        setenv("SECRET", secret, 1);            // set the environment var
        system(exepath);   
    }
    secret[0] = (char)NULL; // remove the used char
    DEBUG && cout << "----finished warming up..." << endl;


    for (int i = 0; i <= MAX_PASS_LENGTH; i++)
    // for (int i = 0; i <= 0; i++)
    // for (int i = 0; i <= 3; i++)
    {
        vector<int64_t> timeTakenVec;   // stores the time it took for system(`vulnerable program`) to return for a character guess
        int64_t longestTime = INT64_MIN;    // records the time taken for system() to return when guessing a character
        int64_t sumTime = 0;
        char bestCharGuess = 'z';           // records the character that caused the longest time taken for system() to return
        DEBUG && cout << "trying to guess hardcoded char at index: " << i << endl; 
        // guess characters
        for (auto charIt = validPasswordChars.begin(); charIt < validPasswordChars.end(); charIt++)
        {
            secret[i] = *charIt;    // test this character
            
            setenv("SECRET", secret, 1);            // set the environment var
            auto time1 = chrono::high_resolution_clock::now();  // record start time of system() call
            system(exepath);
            auto time2 = chrono::high_resolution_clock::now();  // record end time of system() call
            int64_t timeTaken = chrono::duration_cast<chrono::milliseconds>(time2 - time1).count(); // convert time in nanoseconds to milliseconds
            DEBUG && cout << "sys call took: " << timeTaken << "ms | for char: '" << *charIt << "'" << endl;
            timeTakenVec.emplace_back(timeTaken);
            
            sumTime += timeTaken;
            int64_t avgTime = sumTime / timeTakenVec.size() - 1;
            int64_t margin = (avgTime * 5) / 100; // calculate 5% of the average time taken of system() calls

            // shorter the time taken, highly probable that it's the incorrect character,
            // longer the time taken, more probable that is the correct character
            if (timeTaken >= longestTime)
            {
                longestTime = timeTaken;    // update the longest time taken by system() so far
                bestCharGuess = *charIt;    // update the best guess character
            }

            DEBUG && cout << "\tlongest time = " << longestTime << "ms | avgTime = " << avgTime << "ms | 5% of avgTime = " << margin  << "ms" << endl;
            if (longestTime - avgTime > margin)
            {
                DEBUG && cout << "---gonna break, found an anomoly! char: '" << secret[i] << "'" << endl;
                break;
            }
        }

        // if of `longestTime` is within 1% of `avgTime` then we've found the hardcoded password!
        int64_t avgTime = sumTime / timeTakenVec.size() - 1;
        int64_t margin = avgTime / 100;         // calculate 1% of the average time taken of system() calls
        if (longestTime - avgTime <= margin)
        {
            secret[i] = (char)NULL; // remove the unneccessary character
            DEBUG && cout << "---returning, found the hardcoded password! pass: '" << secret << "'\n";
            DEBUG && cout << "\tlongest time = " << longestTime << " | avgTime = " << avgTime << "ms | 1% of avgTime = " << margin  << "ms" << endl;
            return secret;          // return the hardcoded password found
        }
        secret[i] = bestCharGuess;  // update the secret[i] to our best character guess

        // // // // OLD
        // // // find the longest time for system() to return (also finds the character that caused it)
        // // int j = 0;
        // // for (auto it = timeTakenVec.begin(); it <= timeTakenVec.end(); it++, j++)
        // // {
        // //     if (*it >= longestTime)
        // //         longestTime = *it;
        // // }
        // // secret[i] = validPasswordChars[j];  // set the character that caused the longest time for system() to return

        // // calculate the average time taken (minus the longest time taken) for system() calls
        // int64_t avgTime = 0;
        // // for (auto it = timeTakenVec.begin(); it < timeTakenVec.end(); it++)
        // // {
        // //     if (*it == longestTime)
        // //         continue;
        // //     avgTime += *it;
        // // }
        // for (auto time : timeTakenVec)
        // {
        //     if (time == longestTime)
        //         continue;
        //     avgTime += time;
        // }
        // avgTime = avgTime / (timeTakenVec.size() - 1);

        // if (DEBUG)
        // {
        //     // print times
        //     int k = 0;
        //     cout << "printing time taken vec: \n"; 
        //     for (auto time : timeTakenVec)
        //     {
        //         cout << "\ttime: " << time << "ms | char: '" << validPasswordChars[k] << "'\n";
        //         k++;
        //     }
        //     cout << "average time of system() calls: " << avgTime << "ms\n";
        //     cout << "longest time taken was: " << longestTime << "ms\n";
        //     cout << "character that caused it was: `" << secret[i] << "'" << endl; 
        //     cout << "secret so far: `" << secret << "'" << endl; 
        //     // cout << "character that caused it was: `" << validPasswordChars[j] << "'" << endl; 
        //     // cout << "character that caused it was: `" << bestCharGuess << "'" << endl; 
        // }

        // // if the `longestTime` is within 1% of `avgTime` then we've found the 
        // // secret (but remove the last character we've just tred) and should exit
        // int64_t margin = avgTime / 100; // 1% margin of `avgTime`
        // DEBUG && cout << "1% of "<< avgTime << " is: " << margin << endl;

        // // if the `longestTime` - `avgTime` is within `margin`, it means
        // // all the characters we tried for this index (i) were not part of the hardcoded password,
        // // in other words, the the hardcoded password is `secret` with the last char dropped
        // if (longestTime - avgTime <= margin)
        // {
        //     secret[i] = (char)NULL;
        //     DEBUG && cout << "hardcoded password found: " << secret << endl;
        //     return secret;
        // }
    }    
    
    return NULL;
}
