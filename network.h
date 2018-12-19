#pragma once

/**
 * @file network.h
 * @brief Every network_* operation
 *
 * @author Valérian Rousset
 */
#include <stdio.h>
#include <stdlib.h>
#include "client.h"
#include "hashtable.h"
#include "node_list.h"
#include <string.h>

/**
 * @brief get a value from the network
 * @param client client to use
 * @param key key of what we want to find value
 * @param value value to write to, of size MAX_MSG_ELEM_SIZE + 1
 * @return an error code
 */
error_code network_get(client_t client, pps_key_t key, pps_value_t *value);

/**
 * @brief put a value in the network
 * @param client client to use
 * @param key key to retrieve value later
 * @param value value to add
 * @return an error code
 */
error_code network_put(client_t client, pps_key_t key, pps_value_t value);

/**
 * @brief delete a key in the network
 * @param client client to use
 * @param key key to delete
 * @return an error code
 */
error_code network_del(client_t client, pps_key_t key);

