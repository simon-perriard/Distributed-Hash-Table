/**
 * @file system.c
 * @brief Implementation of system.h
 *
 * @author Luis D. Pedrosa
 */

#include <string.h> // for memset
#include <sys/socket.h> // for sockets
#include <netinet/in.h> // for IPPROTO_UDP
#include <sys/time.h> // for struct timeval
#include <arpa/inet.h> // for inet_pton

#include "error.h"
#include "system.h"

// ======================================================================
int get_socket(time_t t)
{
    int fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (fd == -1) return fd;
    debug_print("get_socket(): fd=%d", fd);

    if (t > 0) {
        // Set receive timeout.
        struct timeval timeout;
        memset(&timeout, 0, sizeof(timeout));
        timeout.tv_sec = t;
        int error = setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO,
                               &timeout, sizeof(timeout));
        if (error == -1) fd = -1;
    }

    return fd;
}

// ======================================================================
error_code get_server_addr(const char *ip, uint16_t port, struct sockaddr_in *p_server_addr)
{
    M_REQUIRE_NON_NULL(ip);
    M_REQUIRE_NON_NULL(p_server_addr);

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);

    if (inet_pton(AF_INET, ip, &server_addr.sin_addr) != 1)
        return ERR_NETWORK;

    // only affect when no error
    *p_server_addr = server_addr;

    return ERR_NONE;
}

// ======================================================================
error_code bind_server(int socket, const char *ip, uint16_t port)
{
    struct sockaddr_in server_addr;
    int err = get_server_addr(ip, port, &server_addr);
    if (err != ERR_NONE) return err;

    // Bind to the IP:port.
    if (bind(socket,
             (const struct sockaddr *) &server_addr, sizeof(server_addr)) == -1)
        return ERR_NETWORK;

    return ERR_NONE;
}
