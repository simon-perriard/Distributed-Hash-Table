#include "hashtable.h"
#include <stdio.h>
#include "util.h"


void delete_bucket(bucket_t* b);

void delete_bucket_content(bucket_t* const b);

int initialize_bucket_list(bucket_t* list, const size_t size);

error_code add_Htable_value(Htable_t table, pps_key_t key, pps_value_t value)
{
    M_EXIT_IF(table.list == NULL || value == NULL, ERR_BAD_PARAMETER,"NULL POINTER  in hashtable.c", ,);

    bucket_t* b = &table.list[hash_function(key,table.size)];	//get the bucket via the hash function

    if(b != NULL) {

        if(0 == b->size) {	//no collision
            b->pairs[0].value = strdup(value);	//  set the value	(both already allocated in pps-launch-server)
            b->pairs[0].key = strdup(key);		//  set the key
            b->size = 1;
        } else {

            int found = 0;
            int i = 0;

            while( i < b->size && found != 1) {	//if there is collisions check which one has the corresponding key

                if(strcmp(b->pairs[i].key,key) == 0) {	//update the values

                    b->pairs[i].value = strdup(value);			//the memory is already allocated (just after receiving the value)
                    b->pairs[i].key = strdup(key);
                    found = 1;
                }

                ++i;
            }

            if(0 == found) {	// add new collision

                b->pairs = realloc(b->pairs, (b->size + 1) * sizeof(kv_pair_t));		//allocate more memory
                M_EXIT_IF(b->pairs == NULL, ERR_NOMEM," memory reallocation failed  in hashtable.c", ,);
                kv_pair_t new_pair = {NULL,NULL};
                b->pairs[b->size] = new_pair;
                b->pairs[b->size].key = strdup(key);		//set the values (already allocated)
                b->pairs[b->size].value = strdup(value);

                b->size += 1;
            }
        }
    }

    free_const_ptr(key);
    free_const_ptr(value);

    return ERR_NONE;
}


pps_value_t get_Htable_value(Htable_t table, pps_key_t key)
{
    bucket_t* b = &table.list[hash_function(key,table.size)];	//get the bucket via the hash function
    if(b != NULL) {

        for(size_t i = 0; i < b->size; ++i) {	//if there is collisions check which one has the corresponding key
            if(strcmp(b->pairs[i].key,key) == 0) {
                return strdup(b->pairs[i].value);	//return the value
            }
        }
    }

    return NULL;
}

/** ----------------------------------------------------------------------
** Hash a string for a given hashtable size.
** See http://en.wikipedia.org/wiki/Jenkins_hash_function
**/
size_t hash_function(pps_key_t key, size_t size)
{
    M_REQUIRE(size != 0, SIZE_MAX, "size == %d", 0);
    M_REQUIRE_NON_NULL_CUSTOM_ERR(key, SIZE_MAX);

    size_t hash = 0;
    const size_t key_len = strlen(key);
    for
    (size_t i = 0; i < key_len; ++i) {
        hash += (unsigned char) key[i];
        hash += (hash << 10);
        hash ^= (hash >>  6);
    }
    hash += (hash <<  3);
    hash ^= (hash >> 11);
    hash += (hash << 15);
    return hash % size;
}


Htable_t construct_Htable(size_t size)
{

    Htable_t table = {NULL,0};

    bucket_t* list = calloc(size, sizeof(bucket_t));

    initialize_bucket_list(list, size);

    if(list != NULL && size >= 0) {
        table.list = list;
        table.size = size;
    }

    return table;
}

void delete_Htable_and_content(Htable_t* table)
{

    if(table != NULL) {

        for(size_t i = 0; i < table->size; ++i) {
            delete_bucket(&table->list[i]);
        }

        if(table != NULL) {
            free(table->list);
            table->list = NULL;
        }
    }
}


void delete_bucket(bucket_t* b)
{
    if(b != NULL) {
        delete_bucket_content(b);
        free(b->pairs);
        b->pairs = NULL;
    }
}


void delete_bucket_content(bucket_t* const b)
{

    for(size_t i = 0; i < b->size; ++i) {
        kv_pair_free(&b->pairs[i]);
    }
}


void kv_pair_free(kv_pair_t *kv)
{
    free_const_ptr(kv->key);
    kv->key = NULL;
    free_const_ptr(kv->value);
    kv->value = NULL;
}


int initialize_bucket_list(bucket_t* list, const size_t size)
{

    for(size_t i = 0; i < size; ++i) {

        list[i].pairs = malloc(sizeof(kv_pair_t));		//initialize pointer for the pair
        if(list[i].pairs != NULL) {
            list[i].size = 0;
            list[i].pairs[0].key = malloc(0);
            list[i].pairs[0].value = malloc(0);

            if(list[i].pairs[0].key == NULL || list[i].pairs[0].value == NULL) {
                return -1;
            }
        } else {
            return -1;
        }
    }

    return 0;
}

void kv_list_free(kv_list_t *list)
{
    if(list != NULL) {
        for(size_t i = 0; i < list->size; i++) {
            kv_pair_free(list->list[i]);
        }
    }
    list->list = NULL;
    list->size = 0;
}

kv_list_t *get_Htable_content(Htable_t table)
{

    kv_pair_t ** total_list = malloc(0);
    size_t list_size = 0;
    if(total_list == NULL) {
        return NULL;
    }

    for(size_t i = 0; i < table.size; ++i) {

        for(size_t j = 0; j < table.list[i].size; ++j) {

            total_list = realloc(total_list, ++list_size);
            if(total_list == NULL) {
                return NULL;
            }
            total_list[list_size -1] = &table.list[i].pairs[j];

        }
    }

    kv_list_t* new_list = malloc(sizeof(kv_list_t));
    if(new_list == NULL) {
        return NULL;
    }

    new_list->size = list_size;
    new_list->list = total_list;
    return new_list;

}
