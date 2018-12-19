#pragma once

/**
 * @file hashtable.h
 * @brief Local hash-table implementation.
 *
 * @author Valérian Rousset
 */

#include <stddef.h> // for size_t
#include <stdint.h> // for int32_t (weeks 04 to 06)

#include "error.h" // for error_code
#include <math.h>
#include <stdlib.h>

#define HTABLE_SIZE 256

/*
 * Definition of type for local hash-tables keys
 */
typedef const char* pps_key_t;

/*
 * Definition of type for local hash-tables values
 */
typedef const char* pps_value_t;

/*
 * key-value pair
 */
typedef struct{
    pps_key_t key;
    pps_value_t value;
} kv_pair_t;

/*
 * Definition of type for local hash-tables buckets
 */
typedef struct{
	size_t size;
	kv_pair_t* pairs;		//list of pairs
} bucket_t;


/*
 * Definition of local hash-table type
 */
typedef struct{
	bucket_t* list;
	size_t size;
}Htable_t;



// Only from week 07
#define NO_HTABLE ((Htable_t) NULL)

typedef struct {
	kv_pair_t** list;
	size_t size;
}kv_list_t;

/**
 * @brief construct a hash-table of the given size.
 *    Note: does NOTHING until week 07.
 * @param size number of buckets in the new hash-table
 * @return the newly allocated hash-table
 */

Htable_t construct_Htable(size_t size);

/**
 * @brief delete the given hash-table
 *    Note: does NOTHING until week 07.
 * @param table the hash-table to free (passed by reference)
 */
void delete_Htable_and_content(Htable_t* table);

/**
 * @brief add a key:value pair to hash-table
 * @param table the table where to add
 * @param key the key to which the value shall be associated
 * @param value the value to be added
 * @return 0 on success; error code on error (see error.h)
 */
error_code add_Htable_value(Htable_t table, pps_key_t key, pps_value_t value);

/**
 * @brief get a value for a given in the given hash-table
 * @param table the table where to get
 * @param key the key associated to the wanted value
 * @return the associated value
 */
pps_value_t get_Htable_value(Htable_t table, pps_key_t key);

/**
 * @brief compute the hash for the given key and size of hash-table.
 *      Note: although this is a local function, it is exposed here
 *            for test/grading purposes.
 * @param key the key onto compute the hash
 * @param table_size size of the containing table
 * @return a hash value in range [0..table_size-1], or SIZE_MAX if error
 */
size_t hash_function(pps_key_t key, size_t table_size);

/**
 * @brief get the content of the table
 *    Note: does NOTHING until week 08.
 * @param table table to read from
 * @return the list of key values from the given table, NULL if a failure occured
 */
kv_list_t *get_Htable_content(Htable_t table);

/**
 * @brief delete a key:value pair from the hash-table
 *    Note: does NOTHING until week 10.
 * @param table the table where to delete
 * @param key the key which is to be delete
 * @return 0 on success; error code on errror (see error.h)
 */
error_code del_Htable_key(Htable_t table, pps_key_t key);

/**
 * @brief free a key-value pair content (both of them)
 *    Note: does NOTHING until week 07.
 * @param kv the key-value pair to free
 */
void kv_pair_free(kv_pair_t *kv);

/**
 * @brief free a kv_list and it's content
 *    Note: does NOTHING until week 08.
 * @param list the list to free
 */
void kv_list_free(kv_list_t *list);
