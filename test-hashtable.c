/**
 * @file test-hashtable.c
 * @brief test code for hashtables
 *
 * @author Valérian Rousset & Jean-Cédric Chappelier
 * @date 02 Oct 2017
 */

#include <stdio.h> // for puts(). to be removed when no longer needed.

#include <check.h>

#include "tests.h"
#include "hashtable.h"

START_TEST(add_value_does_retrieve_same_value)
{
    Htable_t h = construct_Htable(3);
    error_code err = add_Htable_value(h, NULL,NULL);

    ck_assert_bad_param(err);
}
END_TEST

Suite *hashtable_suite()
{

    Suite *s = suite_create("hashtable.h");

    TCase *tc_ht = tcase_create("hashtable");
    suite_add_tcase(s, tc_ht);

    tcase_add_test(tc_ht, add_value_does_retrieve_same_value);

    return s;
}

TEST_SUITE(hashtable_suite)
