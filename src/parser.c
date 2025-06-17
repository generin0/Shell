// parser.c
#define _POSIX_C_SOURCE 200809L
#include <stdlib.h>
#include <string.h>

#include "parser.h"

char **parse_input(const char *input, int *argc_out)
{
    if (!input) {
        if (argc_out) *argc_out = 0;
        return NULL;
    }

    /* Make a working copy because strtok modifies the string */
    char *work = strdup(input);
    if (!work)
        return NULL;

    size_t capacity = 8;
    size_t count = 0;
    char **args = malloc(capacity * sizeof(char *));
    if (!args) {
        free(work);
        return NULL;
    }

    char *saveptr = NULL;
    char *token = strtok_r(work, " \t", &saveptr);
    while (token) {
        if (count + 1 >= capacity) { /* +1 for final NULL */
            capacity *= 2;
            char **tmp = realloc(args, capacity * sizeof(char *));
            if (!tmp) {
                /* failure: free everything and abort */
                for (size_t i = 0; i < count; ++i) free(args[i]);
                free(args);
                free(work);
                return NULL;
            }
            args = tmp;
        }
        args[count++] = strdup(token);
        token = strtok_r(NULL, " \t", &saveptr);
    }

    args[count] = NULL;
    if (argc_out) *argc_out = (int)count;
    free(work);
    return args;
} 