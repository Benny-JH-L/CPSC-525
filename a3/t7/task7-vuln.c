/// =========================================================================
/// Copyright (C) 2025 Pavol Federl (pfederl@ucalgary.ca)
/// All Rights Reserved. Do not distribute this file.
/// =========================================================================
#include <error.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef SECRET
#error "you forgot to compile with -DSECRET=password"
#endif

#ifndef MESSAGE
#error "you forgot to compile with -DMESSAGE='hidden message'"
#endif
#define QUOTE__(...) #__VA_ARGS__
#define QUOTE(x) QUOTE__(x)

static const char * password = QUOTE(SECRET);

void debug_unused()
{
    printf("password = '%s'\n", password);
    fflush(stdout);
}

static void get_pass(char * buff)
{
    printf("Password:");
    for (char * p = buff ; ; p++) {
        int c = fgetc(stdin);
        // printf("Read char: (0x%02x)\n", c);
        if (c == EOF && p == buff) { exit(0); }
        if (c == EOF || c == '\n') {
            *p = '\0';
            return;
        }
        else {
            *p = c;
        }
    }
}

static void simulate_gdb(
    void * builtin_ret_addr, void * bultin_frame_addr, void * buff_addr,
    void * debug_unused_addr)
{
    void ** retaddr_uint64 = (void **) bultin_frame_addr;
    if (retaddr_uint64[1] != builtin_ret_addr) {
        error(-1, 0, "Stack frame layout is not as expected.");
    }
    printf("DEBUG: simulated gdb's output\n");
    printf("DEBUG: (gdb) start\n");
    printf("DEBUG: (gdb) info frame:\n");
    printf("DEBUG:   saved rip = %p\n", builtin_ret_addr);
    printf("DEBUG:   Saved registers:\n");
    printf(
        "DEBUG:     rbp at %p, rip at %p\n", &retaddr_uint64[0],
        &retaddr_uint64[1]);
    printf("DEBUG: (gdb) print/x &buff\n");
    printf("DEBUG: $1 = %p\n", buff_addr);
    printf("DEBUG: (gdb) print/x &debug_unused\n");
    printf("DEBUG: $2 = %p\n", debug_unused_addr);
    printf("DEBUG: (gdb) end\n");
}

int main()
{
    int debug = getenv(":)") != NULL;

    char buff[20];

    if (debug) {
        simulate_gdb(
            __builtin_return_address(0), __builtin_frame_address(0), &buff,
            debug_unused);
    }

    get_pass(buff);
    if (strcmp(buff, password) != 0) {
        printf("Wrong password.\n");
        return 0;
    }

    printf("-----BEGIN HIDDEN MESSAGE-----\n");
    printf(QUOTE(MESSAGE) "\n");
    printf("-----END HIDDEN MESSAGE-----\n");

    return 0;
}
