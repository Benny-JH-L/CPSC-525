/// =========================================================================
/// Copyright (C) 2025 Pavol Federl (pfederl@ucalgary.ca)
/// All Rights Reserved. Do not distribute this file.
/// =========================================================================

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef SECRET
#error "you must compile with -DSECRET=password"
#endif
#ifndef MESSAGE
#error "you must compile with -DMESSAGE='some secret message'"
#endif
#define QUOTE__(...) #__VA_ARGS__
#define QUOTE(x) QUOTE__(x)

const char * hardcoded_password = QUOTE(SECRET);

static void show_secret_message()
{
    printf("-----BEGIN HIDDEN MESSAGE-----\n");
    printf(QUOTE(MESSAGE) "\n");
    printf("-----END HIDDEN MESSAGE-----\n");
}

static const char * get_pass()
{
    printf("Password:");
    static char buff[32] = {0};
    char * res = fgets(buff, sizeof(buff), stdin);
    if (res == NULL) strcpy(buff, "");
    size_t len = strlen(buff);
    if (len > 0 && buff[len - 1] == '\n') buff[--len] = 0;
    return buff;
}

static int DEBUG = 0;

static int compare_passwords(const char * p1, const char * p2)
{
    size_t len = strlen(p1);
    if (len != strlen(p2)) return 1;
    int n_differences = 0;
    for (size_t i = 0; i < len; i++) {
        n_differences += p1[i] != p2[i];
        DEBUG && printf("%d ", n_differences);
    }
    DEBUG && printf("\n");
    return n_differences;
}

int main()
{
    DEBUG = getenv(" D E B U G ") != NULL;
    if (DEBUG) printf("Debug mode active.\n", DEBUG);
    const char * pass = get_pass();
    if (compare_passwords(pass, hardcoded_password) != 0) {
        printf("Wrong password.\n");
        return 0;
    }
    show_secret_message();
    return 0;
}
