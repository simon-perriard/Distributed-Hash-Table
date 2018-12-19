#pragma once

/**
 * @file error.h
 * @brief Error codes for PPS-DHT project
 *
 * @author E. Bugnion, J.-C. Chappelier, Valérian Rousset
 * @date summer 2016
 */

#ifdef DEBUG
#include <stdio.h> // for fprintf
#endif
#include <string.h> // strerror()
#include <errno.h>  // errno

#ifdef __cplusplus
extern "C" {
#endif

// ======================================================================
/**
 * @brief internal error codes.
 *
 */
typedef enum {
    ERR_NONE = 0, // no error
    ERR_NOMEM,
    ERR_IO,
    ERR_BAD_PARAMETER,
    ERR_NETWORK,
    ERR_NOT_FOUND,
    ERR_LAST // not an actual error but to have e.g. the total number of errors
} error_code;

// ======================================================================
/*
 * Helpers (macros)
 */

// ----------------------------------------------------------------------
/**
 * @brief debug_print macro is usefull to print message in DEBUG mode only.
 */
#ifdef DEBUG
#define debug_print(fmt, ...) \
    fprintf(stderr, __FILE__ ":%d:%s: " fmt "\n", \
       __LINE__, __func__, __VA_ARGS__)
#else
#define debug_print(fmt, ...) \
    do {} while(0)
#endif

// ----------------------------------------------------------------------
/**
 * @brief M_EXIT macro is usefull to return an error code from a function with a debug message.
 *        Example usage:
 *           M_EXIT(ERR_BAD_PARAMETER, "unable to do something decent with value %lu", i);
 */
#define M_EXIT(error_code, fmt, ...)  \
    do { \
        debug_print(fmt, __VA_ARGS__); \
        return error_code; \
    } while(0)

// ----------------------------------------------------------------------
/**
 * @brief M_EXIT_ERR_NOMSG macro is a kind of M_EXIT, indicating (in debug mode)
 *        only a name and the error message corresponding to the error code.
 *        The name parameter is typically the name of the program.
 */
#define M_EXIT_ERR_NOMSG(error_code, name)   \
    M_EXIT(error_code, "%s: error: %s", name, ERR_MESSAGES[error_code - ERR_NONE])

// ----------------------------------------------------------------------
/**
 * @brief M_EXIT_ERR macro is a kind of M_EXIT, indicating (in debug mode)
 *        the error message corresponding to the error code, followed by the message
 *        passed in argument.
 *        Example usage:
 *            M_EXIT_ERR(ERR_BAD_PARAMETER, c.name, "unable to generate from %lu", i);
 */
#define M_EXIT_ERR(error_code, name, fmt, ...)   \
    M_EXIT(error_code, "%s: error: %s" fmt, name, ERR_MESSAGES[error_code - ERR_NONE], __VA_ARGS__)

// ----------------------------------------------------------------------
/**
 * @brief M_EXIT_IF macro calls M_EXIT_ERR only if providef test is true.
 */
#define M_EXIT_IF(test, error_code, name, fmt, ...)   \
    do { \
        if (test) { \
            M_EXIT_ERR(error_code, name, fmt, __VA_ARGS__); \
        } \
    } while(0)

// ----------------------------------------------------------------------
/**
 * @brief M_EXIT_IF_ERR macro is a shortcut for M_EXIT_IF testing if some error occured,
 *        i.e. if error `ret` is different from ERR_NONE.
 */
#define M_EXIT_IF_ERR(ret, name)   \
    M_EXIT_IF(ret != ERR_NONE, ret, name, ", at line %d", __LINE__)

// ----------------------------------------------------------------------
/**
 * @brief M_EXIT_IF_NULL macro is usefull to warn (and stop) when NULL pointers are detected.
 *        size is typically the allocation size and name the name of the program.
 */
#define M_EXIT_IF_NULL(var, size, name)   \
    M_EXIT_IF(var == NULL, ERR_NOMEM, name, ", cannot allocate %d bytes for %s", size, #var)

// ----------------------------------------------------------------------
/**
 * @brief M_EXIT_IF_TOO_LONG macro is usefull to warn (and stop) when a string is more than size characters long.
 */
#define M_EXIT_IF_TOO_LONG(var, size, name)   \
    M_EXIT_IF(strlen(var) > size, ERR_BAD_PARAMETER, name, ", given %s is larger than %d bytes", #var, size)

// ----------------------------------------------------------------------
/**
 * @brief M_EXIT_IF_TRAILING macro is usefull to test whether some trailing character(s) remain(s) in file.
 */
#define M_EXIT_IF_TRAILING(file, name)   \
    M_EXIT_IF(getc(file) != '\n', ERR_BAD_PARAMETER, name, ", trailing chars on \"%s\"", #file)

// ----------------------------------------------------------------------
/**
 * @brief M_EXIT_IF_TRAILING_WITH_CODE macro is similar to M_EXIT_IF_TRAILING but allows to
 *        run some code before exiting.
 *        Example usage:
 *           M_EXIT_IF_TRAILING_WITH_CODE(stdin, progname, { free(something); } );
 */
#define M_EXIT_IF_TRAILING_WITH_CODE(file, name, code)   \
    do { \
        if (getc(file) != '\n') { \
            code; \
            M_EXIT_ERR(ERR_BAD_PARAMETER, name, ", trailing chars on \"%s\"", #file); \
        } \
    } while(0)

// ----------------------------------------------------------------------
/**
 * @brief M_REQUIRE macro is similar to M_EXIT_IF but with two differences:
 *         1) making use of the NEGATION of the test (thus "require")
 *         2) making use of M_EXIT rather than M_EXIT_ERR
 */
#define M_REQUIRE(test, error_code, fmt, ...)   \
    do { \
        if (!(test)) { \
             M_EXIT(error_code, fmt, __VA_ARGS__); \
        } \
    } while(0)

// ----------------------------------------------------------------------
/**
 * @brief M_REQUIRE_NO_ERRNO macro is a M_REQUIRE test on errno.
 *        This might be useful to change errno into our own error code.
 */
#define M_REQUIRE_NO_ERRNO(error_code) \
    M_REQUIRE(errno == 0, error_code, "%s", strerror(errno))

// ----------------------------------------------------------------------
/**
 * @brief M_REQUIRE_NON_NULL_CUSTOM_ERR macro is usefull to check for non NULL argument.
 *        Example usage:
 *            M_REQUIRE_NON_NULL_CUSTOM_ERR(key, NULL);
 */
#define M_REQUIRE_NON_NULL_CUSTOM_ERR(arg, error_code) \
    M_REQUIRE(arg != NULL, error_code, "parameter %s is NULL", #arg)

// ----------------------------------------------------------------------
/**
 * @brief M_REQUIRE_NON_NULL macro is a shortcut for
 *        M_REQUIRE_NON_NULL_CUSTOM_ERR returning ERR_BAD_PARAMETER.
 */
#define M_REQUIRE_NON_NULL(arg) \
    M_REQUIRE_NON_NULL_CUSTOM_ERR(arg, ERR_BAD_PARAMETER)

// ======================================================================
/**
* @brief internal error messages. defined in error.c
*
*/
extern
const char * const ERR_MESSAGES[];

#ifdef __cplusplus
}
#endif
