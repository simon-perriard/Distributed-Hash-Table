#define _GNU_SOURCE
#include <arpa/inet.h>
#include <assert.h>
#include <dlfcn.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/file.h>
#include <sys/socket.h>
#include <sys/types.h>

#define LOG_FILE "packets.log"

#define PASS_THROUGH(fn, ...)                                                  \
  (((__typeof__(fn) *)dlsym(RTLD_NEXT, #fn))(__VA_ARGS__))

ssize_t sendto(int socket, const void *message, size_t length, int flags,
               const struct sockaddr *dest_addr, socklen_t dest_len)
{
    const char *delay_str = getenv("SEND_DELAY");
    if (delay_str) {
        usleep(atoi(delay_str) * 1000);
    }

    FILE *log = fopen(LOG_FILE, "a");
    assert(log && "Unable to open packet log file.");

    assert(flock(fileno(log), LOCK_EX) == 0 && "Unable to lock log file.");

    ssize_t result =
        PASS_THROUGH(sendto, socket, message, length, flags, dest_addr, dest_len);

    struct sockaddr_in bind_addr;
    socklen_t bind_len = sizeof(bind_addr);
    assert(getsockname(socket, &bind_addr, &bind_len) == 0);

    char str[INET_ADDRSTRLEN];
    fprintf(log, "%s %d",
            inet_ntop(AF_INET, &bind_addr.sin_addr, str, INET_ADDRSTRLEN),
            ntohs(bind_addr.sin_port));
    fprintf(log, " %s %d",
            inet_ntop(AF_INET, &(((struct sockaddr_in *)dest_addr)->sin_addr),
                      str, INET_ADDRSTRLEN),
            ntohs(((struct sockaddr_in *)dest_addr)->sin_port));
    for (int i = 0; i < length; i++) {
        fprintf(log, " %02X", ((uint8_t *)message)[i]);
    }
    fprintf(log, "\n");
    fflush(log);

    assert(flock(fileno(log), LOCK_UN) == 0 && "Unable to unlock log file.");

    fclose(log);

    return result;
}
