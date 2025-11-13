/// =========================================================================
/// Copyright (C) 2025 Pavol Federl (pfederl@ucalgary.ca)
/// All Rights Reserved. Do not distribute this file.
/// =========================================================================

#include <assert.h>
#include <error.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef SECRET
#error you forgot to compile with -DSECRET=start,len
#endif

#ifndef MESSAGE
#error "you forgot to compile with '-DMESSAGE=some secret message'"
#endif
#define QUOTE__(...) #__VA_ARGS__
#define QUOTE(x) QUOTE__(x)

// The hardcoded password is stored as an index + length into giant_string[],
// but only alphanumeric characters are part of the password in this range
const size_t secret_pos[2] = {SECRET};

// hidden message stored 'encrypted' to prevent use of strings(1)
//   - encryption is simple XOR with 170
// the included memfrob.py can be used to generate the obfuscated message, e.g.
// gcc -g -DMESSAGE=`python3 memfrob.py "some secret message"` \
//        -DSECRET=1,10 task3-vuln.c
const char * hidden_obfuscated_message = QUOTE(MESSAGE);

// this giant string will be used as the source of the password characters
// (it is defined at the end of the file)
const char * giant_string;

// buffer to hold entered password
char entered_password[32];

// helper function to check if a char is alphanumeric
int is_alnum(char c)
{
    return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'z')
        || (c >= 'A' && c <= 'Z');
}

// compare entered_password to the hardcoded password
// if passwords don't match, print "Bad password." and exit(0)
// if passwords match, return normally
void check_passwords()
{
    // extract actual password from giant string
    size_t pw_start = secret_pos[0];
    size_t pw_len = secret_pos[1];
    char stored_password[32] = {0};
    if (pw_len >= sizeof(stored_password)) {
        error(-1, 0, "internal error: stored_password_length too large");
    }
    if (pw_start + pw_len >= strlen(giant_string)) {
        error(-1, 0, "internal error: stored_password exceeds giant_string");
    }
    // copy chars from giant_string, skipping non-alphanumeric chars
    for (size_t src = 0, dst = 0; src < pw_len; src++) {
        char nextchar = giant_string[pw_start + src];
        if (is_alnum(nextchar)) {
            stored_password[dst++] = nextchar;
            stored_password[dst] = 0;
        }
    }
    // compare to entered password
    if (strcmp(entered_password, stored_password) != 0) {
        printf("Bad password.\n");
        exit(0);
    }
}

// Deobfuscate and print the hidden message, and check password before printing
// each char. 
// This makes it bit difficult to retrieve the hidden message through gdb.
void secure_printf(const char * s)
{
    for (size_t i = 0; s[i] != 0; i++) {
        check_passwords();
        putchar(s[i] ^ 170);
    }
}

void show_secret_message()
{
    printf("-----BEGIN HIDDEN MESSAGE-----\n");
    secure_printf(hidden_obfuscated_message);
    printf("\n");
    printf("-----END HIDDEN MESSAGE-----\n");
}

void get_pass()
{
    // prompt user for password, and save it in global entered_password[]
    printf("Password:");
    char * res = fgets(entered_password, sizeof(entered_password), stdin);
    if (res == NULL) strcpy(entered_password, "");
    size_t len = strlen(entered_password);
    if (len > 0 && entered_password[len - 1] == '\n')
        entered_password[--len] = 0;
}

int main()
{
    get_pass();
    check_passwords();
    show_secret_message();
    return 0;
}

const char * giant_string = "\
Security through obscurity From Wikipedia, the free encyclopedia \
Security through obscurity should not be used as the only security \
feature of a system.  In security engineering, security through \
obscurity is the practice of concealing the details or mechanisms of a \
system to enhance its security. This approach relies on the principle \
of hiding something in plain sight, akin to a magician's sleight of \
hand or the use of camouflage. It diverges from traditional security \
methods, such as physical locks, and is more about obscuring \
information or characteristics to deter potential threats. Examples of \
this practice include disguising sensitive information within \
commonplace items, like a piece of paper in a book, or altering digital \
footprints, such as spoofing a web browser's version number. While not a \
standalone solution, security through obscurity can complement other security \
measures in certain scenarios.[1] Obscurity in the context of security \
engineering is the notion that information can be protected, to a certain \
extent, when it is difficult to access or comprehend. This concept hinges \
on the principle of making the details or workings of a system less visible \
or understandable, thereby reducing the likelihood of unauthorizedaccess \
or manipulation.[2] History An early opponent of security through obscurity \
was the locksmith Alfred Charles Hobbs, who in 1851 demonstrated to the \
public how state-of-the-art locks could be picked. In response to concerns \
that exposing security flaws in the design of locks could make them more \
vulnerable to criminals, he said: 'Rogues are very keen in their \
profession, and know already much more than we can teach them.'[3] \
There is scant formal literature on the issue of security through \
obscurity. Books on security engineering cite Kerckhoffs' doctrine from 1883,\
 if they cite anything at all. For example, in a discussion about secrecy \
 and openness in nuclear command and control: [T]he benefits of reducing \
 the likelihood of an accidental war were considered to outweigh the \
 possible benefits of secrecy. This is a modern reincarnation of \
 Kerckhoffs' doctrine, first put forward in the nineteenth century, \
 that the security of a system should depend on its key, not on its \
 design remaining obscure.[4] Peter Swire has written about the \
 trade-off between the notion that 'security through obscurity is \
 an illusion' and the military notion that 'loose lips sink ships',[5] \
 as well as on how competition affects the incentives to disclose.[6]\
 [further explanation needed] There are conflicting stories about \
 the origin of this term. Fans of MIT's Incompatible Timesharing \
 System (ITS) say it was coined in opposition to Multics users down \
 the hall, for whom security was far more an issue than on ITS. Within \
 the ITS culture the term referred, self-mockingly, to the poor coverage \
 of the documentation and obscurity of many commands, and to the \
 attitude that by the time a tourist figured out how to make trouble \
 he'd generally got over the urge to make it, because he felt part \
 of the community. One instance of deliberate security through obscurity \
 on ITS has been noted: the command to allow patching the running \
 ITS system (altmode altmode control-R) echoed as $$^D. Typing Alt \
 Alt Control-D set a flag that would prevent patching the system \
 even if the user later got it right.[7]\
";
// source: https://en.wikipedia.org/wiki/Security_through_obscurity (2024-06)