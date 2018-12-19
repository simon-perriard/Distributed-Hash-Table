/**
 * @file util.c
 * @brief Some tool functions
 *
 * @author Valérian Rousset
 */

#include <stdlib.h>
#include <string.h>

// ======================================================================
char *strdup(const char *str)
{
    if (str == NULL)
        return NULL;

    const size_t size = strlen(str) + 1;
    char *ret = malloc(size);

    if (ret != NULL)
        memcpy(ret, str, size);

    return ret;
}

// ======================================================================
size_t argv_size(char **argv)
{
    char **ptr = argv;
    while (*ptr != NULL) ++ptr;
    return (size_t) (ptr - argv); // we know ptr >= argv
}
