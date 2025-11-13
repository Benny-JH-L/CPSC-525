/// =========================================================================
/// Copyright (C) 2025 Pavol Federl (pfederl@ucalgary.ca)
/// All Rights Reserved. Do not distribute this file.
/// =========================================================================

#include <error.h>
#include <openssl/evp.h>
#include <stdio.h>
#include <string.h>

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
    static char resbuf[EVP_MAX_MD_SIZE * 2];
    EVP_MD_CTX * context = EVP_MD_CTX_create();
    if (! context) error(-1, 0, "failed EVP_MD_CTX_create");
    if (! EVP_DigestInit_ex(context, EVP_sha256(), NULL))
        error(-1, 0, "failed EVP_DigestInit_ex");
    if (! EVP_DigestUpdate(context, str, strlen(str)))
        error(-1, 0, "failed EVP_DigestUpdate");
    unsigned char hashBuff[EVP_MAX_MD_SIZE];
    unsigned int hashLen = 0;
    if (! EVP_DigestFinal_ex(context, hashBuff, &hashLen))
        error(-1, 0, "failed EVP_DigestFinal_ex");
    for (unsigned int i = 0; i < hashLen; ++i) {
        sprintf(resbuf + i * 2, "%02x", hashBuff[i]);
    }
    EVP_MD_CTX_destroy(context);
    return resbuf;
}

static const char * get_pass()
{
    printf("Password:");
    static char buff[32] = {0};
    char * res = fgets(buff, sizeof(buff), stdin);
    if (res == NULL) strcpy(buff, "");
    size_t len = strlen(buff);
    if (len > 0 && buff[len - 1] == '\n') 
    {
        buff[--len] = 0;
        printf("yo, %s\n", buff);
    }
    printf("bruhh, %s\n", buff);
    return buff;
}

int main()
{
    int DEBUG = getenv("debughash") != NULL;
    DEBUG && printf("Debug mode active.\n");
    DEBUG && printf("Stored hash: %s\n", stored_pass_hash);
    // ask user for password
    const char * pass = get_pass();
    // calculate hash of entered password
    const char * hash = hashString(pass);
    DEBUG && printf("Calculated hash: %s\n", hash);
    // compare hashes, using strncmp as it's more secure than strcmp
    if (strncmp(hash, stored_pass_hash, EVP_MAX_MD_SIZE >> 4) != 0) {
        printf("Password does not match! Bye.\n");
        return 0;
    }
    show_secret_message();
    return 0;
}
