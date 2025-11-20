/// =========================================================================
/// Copyright (C) 2025 Pavol Federl (pfederl@ucalgary.ca)
/// All Rights Reserved. Do not distribute this file.
/// =========================================================================
///
/// DO NOT MODIFY this file and DO NOT SUBMIT it for grading

#include <cstdio>
#include <error.h>

const char * guess4(const char * exepath);

int main(int argc, char ** argv)
{
    if (argc != 2) error(-1, 0, "missing executable argument");

    printf("guessing password in %s\n", argv[1]);
    const char * result = guess4(argv[1]);
    if (result) { printf("password='%s'\n", result); }
    else {
        printf("could not find the password\n");
    }
    return 0;
}
