#pragma once

/**
 * @file util.h
 * @brief Some tool tools/functions
 *
 * @author Valérian Rousset
 */

#include <stddef.h> // for size_t

/**
 * @brief tag a variable as POTENTIALLY unused, to avoid compiler warnings
 */
#define _unused __attribute__((unused))

/**
 * @brief useful to free pointers to const without warning. Use with care!
 */
#define free_const_ptr(X) free((void*)X)

/**
 * @brief create a new copy of a string
 * @param str string to be copied
 * @return the newly allocated copy
 */
char *strdup(const char *str);

/**
 * @brief computes actual number of arguments
 * @param pointer to argument array
 * @return the number of non NULL arguments after (and including) argv
 */
size_t argv_size(char **argv);
