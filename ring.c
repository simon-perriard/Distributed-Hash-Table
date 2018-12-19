#include "ring.h"
#include <openssl/sha.h>
#include <string.h>

int contains_server(node_list_t* list, node_t* node);

ring_t *ring_alloc()
{
    return get_nodes();
}

error_code ring_init(ring_t *ring)
{

    if(ring == NULL) {
        return ERR_BAD_PARAMETER;
    }

    node_list_sort(ring, node_cmp_sha);


    return ERR_NONE;
}

void ring_free(ring_t *ring)
{
    node_list_free(ring);
}

node_list_t *ring_get_nodes_for_key(const ring_t *ring, size_t wanted_list_size, pps_key_t key)
{
    unsigned char hashed_key[SHA_DIGEST_LENGTH];

    SHA1((const unsigned char*)key, strlen(key), hashed_key);

    size_t considered_nodes = 0;
    size_t start_index = 0;

    while(start_index < ring->size && strcmp((char*)hashed_key, (char*)ring->list[start_index].sha1) >= 0) {

        start_index++;
    }

    start_index = start_index%ring->size;

    node_list_t* result_list = node_list_new();
    if(result_list == NULL) {
        return NULL;
    }

    for(size_t i = start_index; i < ring->size + start_index && considered_nodes < wanted_list_size; i++) {

        if(contains_server(result_list, &ring->list[i%ring->size]) == 0) {

            error_code err = node_list_add(result_list, ring->list[i%ring->size]);
            if(err == ERR_NONE) {
                considered_nodes++;
            }
        }
    }

    if(considered_nodes < wanted_list_size) {
        return NULL;
    }

    result_list->nbre_of_servers = considered_nodes;

    return result_list;
}

int contains_server(node_list_t* list, node_t* node)
{

    for(size_t i = 0; i < list->size; i++) {
        if(node_cmp_server_addr(&list->list[i], node) == 0) {
            return 1;
        }
    }

    return 0;
}

