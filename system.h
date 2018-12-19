#pragma once

/**
 * @file system.h
 * @brief Low level system related stuff (currently: IP/socket)
 *
 * @author Luis D. Pedrosa
 */

#include <stdint.h> // for uint16_t
#include <netinet/in.h> // for sockaddr_in
#include <time.h> // for time_t

#include "error.h"

/**
 * @brief get a socket for communication and sets its reception timeout
 * @param receive timeout in seconds, 0 means infinity (no timeout)
 * @return socket ID
 */
int get_socket(time_t receive_timeout_in_seconds);

/**
 * @brief transform server human-format address to internal format
 * @param ip IP address to connect to
 * @param port port to connect to
 * @param addr where to store the resulting address
 * @return an error code != ERR_NONE if anything went wrong
 */
error_code get_server_addr(const char *ip, uint16_t port, struct sockaddr_in *addr);

/** ======================================================================
 * @brief bind server socket to some IP address and port
 * @param socket socket to be bound
 * @param ip IP address to bind to
 * @param port port to bind to
 * @return an error code != ERR_NONE if anything went wrong
 */
error_code bind_server(int socket, const char *ip, uint16_t port);
