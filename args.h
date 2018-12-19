#pragma once

/**
 * @file args.h
 * @brief Parsing argv options. This is required only from week 10.
 *
 * @author Valérian Rousset
 */

#include <stddef.h> // for size_t

/**
 * @brief containt parsed arguments
 */

typedef struct{
	size_t N;
	size_t R;
	size_t W;
}args_t;

/**
 * @brief enum for the supported_args flag of parse_opt_args
 */
typedef enum {
    TOTAL_SERVERS = 1 << 0,
    GET_NEEDED    = 1 << 1,
    PUT_NEEDED    = 1 << 2
} args_kind;

/**
 * @brief parse optional arguments
 * @param supported_args OR'ed flags of supported options
 * @param rem_argv ref to array of string to check for argument, with last elem == NULL
 * @return malloc'ed user inputed values, default when not set, or NULL altogether, when an error occured
 */
args_t *parse_opt_args(size_t supported_args, char ***rem_argv);

/**
 * @brief checks if a string does not represent an integer
 * @param str the string to check
 * @return 1 if it is not a number, 0 otherwise
 */
int isNaN(const char* str);

/**
 * @brief converts a string representing an integer to an integer
 * @param str the string to convert
 * @param i pointer to the destination
 */
void str_to_int(char* str, size_t* i);
