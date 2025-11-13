/// =========================================================================
/// Copyright (C) 2025 Pavol Federl (pfederl@ucalgary.ca)
/// All Rights Reserved. Do not distribute this file.
/// =========================================================================

#define _GNU_SOURCE
#include <ctype.h>
#include <errno.h>
#include <error.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#ifndef MESSAGE
#error "you forgot to compile with '-DMESSAGE=hidden message'"
#endif
#define QUOTE__(...) #__VA_ARGS__
#define QUOTE(x) QUOTE__(x)

static void show_secret_message()
{
    printf("-----BEGIN HIDDEN MESSAGE-----\n");
    printf(QUOTE(MESSAGE) "\n");
    printf("-----END HIDDEN MESSAGE-----\n");
}

#ifndef SECRET
#error you forgot to compile with -DSECRET=<sha256-hash>
#endif

static const char * stored_pass_hash = QUOTE(SECRET);

static const char * hashString(const char * str)
{
    static char resbuff[65];
    char cmdbuff[4096];
    snprintf(cmdbuff, sizeof(cmdbuff), "echo -n %s | sha256sum", str);
    FILE * fp = popen(cmdbuff, "r");
    if (fp == NULL) error(-1, 0, "popen failed");
    char * res = fgets(resbuff, 65, fp);
    if (res == NULL) error(-1, 0, "fgets failed");
    if (strlen(resbuff) != 64) error(-1, 0, "bad hash length from sha256sum");
    pclose(fp);
    return resbuff;
}

static const char * get_pass()
{
    printf("Password:");
    static char buff[32] = {0};
    char * res = fgets(buff, sizeof(buff), stdin);
    if (res == NULL) strcpy(buff, "");
    size_t len = strlen(buff);
    if (len > 0 && buff[len - 1] == '\n') buff[--len] = 0;
    for (size_t i = 0; i < len; i++)
        if (! isalnum(buff[i]))
            error(-1, 0, "only alphanumeric chars allowed in password");
    return buff;
}

int main()
{
    char log_command[4096];
    snprintf(
        log_command, sizeof(log_command), "log -stored-hash '%s' 2> /dev/null",
        stored_pass_hash);
    system(log_command);

    const char * pass = get_pass();
    const char * hash = hashString(pass);
    if (strcmp(hash, stored_pass_hash) != 0) {
        printf("Password does not match! Bye.\n");
        return 0;
    }
    show_secret_message();
    return 0;
}
