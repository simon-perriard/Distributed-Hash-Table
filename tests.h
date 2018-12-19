#pragma once

/**
 * @file tests.c
 * @brief Utilities for tests
 *
 * @author Valérian Rousset
 */

#include <stdlib.h> // EXIT_FAILURE
#include <check.h>

#include "error.h"

#define ck_assert_bad_param(value) \
	ck_assert_int_eq(value, ERR_BAD_PARAMETER)

#define ck_assert_err_none(value) \
	ck_assert_int_eq(value, ERR_NONE)

#ifndef ck_assert_ptr_nonnull
#define ck_assert_ptr_nonnull(ptr) \
	ck_assert_ptr_ne(ptr, NULL)
#endif

#ifndef ck_assert_ptr_null
#define ck_assert_ptr_null(ptr) \
	ck_assert_ptr_eq(ptr, NULL)
#endif


#define TEST_SUITE(get_suite) \
int main() \
{ \
    SRunner *sr = srunner_create(get_suite()); \
    srunner_run_all(sr, CK_VERBOSE); \
 \
    int number_failed = srunner_ntests_failed(sr); \
    srunner_free(sr); \
 \
    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE; \
}
