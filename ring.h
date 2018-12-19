#pragma once

/**
 * @file ring.h
 * @brief Ring parsing and handling. This is required only from week 11.
 *
 * @author Valérian Rousset
 */

#include <stddef.h>
#include <stdint.h>

#include "error.h"
#include "hashtable.h"
#include "node_list.h"

/*
 * Definition of type for a ring of nodes
 */
typedef node_list_t ring_t;

/**
 * @brief creates a new ring of nodes
 * @return a newly created ring
 */
ring_t *ring_alloc();

/**
 * @brief initializes a ring
 * @param ring the ring to be initialized (modified)
 * @return some error if something wrong during initialization
 */
error_code ring_init(ring_t *ring);

/**
 * @brief destroy a ring of nodes
 * @param ring the ring to be destroyed
 */
void ring_free(ring_t *ring);

/**
 * @brief search nodes storing for a key
 * @param  ring the ring of nodes to search into
 * @param  wanted_list_size minimum of nodes wanted
 * @param  key the key for which we are looking for
 * @return the list of all nodes storing the key
 */
node_list_t *ring_get_nodes_for_key(const ring_t *ring, size_t wanted_list_size, pps_key_t key);
