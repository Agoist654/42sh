#include "io_backend.h"

#define _POSIX_C_SOURCE 200809L

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static FILE *stream = NULL;
static char next = -4;

void io_backend(int argc, char *argv[])
{
    if (argc == 1)
    {
        stream = fdopen(0, "r");
    }
    else if(strcmp(argv[1], "-c") == 0 && argv[2] != NULL)
    {
        stream = fmemopen(argv[2], strlen(argv[2]), "r");
    }
    else
    {
        stream = fopen(argv[1], "r");
    }
    return;
}

char io_peek(void)
{
    if (stream == NULL)
    {
        return EOF;
    }
    if (next == -4)
    {
        next = fgetc(stream);
        return next;
    }
    return next;
}

char io_pop(void)
{
    if (stream == NULL)
    {
        return EOF;
    }
    if (next == -4)
    {
        next = fgetc(stream);
    }
    char ret = next;
    next = fgetc(stream);
    return ret;
}
