#include "node.h"
#include <openssl/sha.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <arpa/inet.h>


#define MAXPORTNUMBER 65535
#define MAX_SIZE_IP 15

char* number_to_string(const size_t number);

error_code node_init(node_t *node, const char *ip, uint16_t port, size_t _unused node_id)
{
    M_REQUIRE_NON_NULL(node);
    M_REQUIRE_NON_NULL(ip);

    char* str_port = number_to_string((size_t)port);
    char* str_id = number_to_string(node_id);

    size_t length = strlen(ip) + strlen(str_port) + strlen(str_id)+2; //+2 for 2 spaces

    node->sha1 = calloc(SHA_DIGEST_LENGTH, sizeof(char));

    if(node->sha1 == NULL) {
        return ERR_NOMEM;
    }

    //construct the string to be hashed
    char data[length];
    memset(data, 0, length);

    strcat(data, ip);

    data[strlen(ip)] = ' ';


    strcat(data+strlen(ip)+1, str_port);

    data[strlen(ip) + 1 + strlen(str_port)] = ' ';

    strcat(data + strlen(ip) + 1 + strlen(str_port) + 1, str_id);


    SHA1((const unsigned char*)data, length, node->sha1);

    node->node_id = node_id;

    return get_server_addr(ip, port, &(node->socket));
}

void node_end(node_t *node)
{
    free(node->sha1);
    node->sha1 = NULL;
}


int node_cmp_sha(const node_t *first, const node_t *second)
{
    return strncmp((char*)first->sha1, (char*)second->sha1, SHA_DIGEST_LENGTH);
}

int node_cmp_server_addr(const node_t *first, const node_t *second)
{
    char ip1[MAX_SIZE_IP+1];
    memset(ip1, 0, MAX_SIZE_IP+1);
    inet_ntop(AF_INET, &first->socket.sin_addr, ip1, MAX_SIZE_IP+1);

    char ip2[MAX_SIZE_IP+1];
    memset(ip2, 0, MAX_SIZE_IP+1);
    inet_ntop(AF_INET, &second->socket.sin_addr, ip2, MAX_SIZE_IP+1);

    if(strcmp(ip1, ip2) != 0) {
        return strcmp(ip1, ip2);
    }

    if(ntohs(first->socket.sin_port) > ntohs(second->socket.sin_port)) {
        return 1;
    }

    if(ntohs(first->socket.sin_port) < ntohs(second->socket.sin_port)) {
        return -1;
    }

    return 0;
}

/**
 * @brief convert an unsigned number to a string (allocated)
 */
char* number_to_string(const size_t number)
{

    char* str = malloc(0);

    if(str == NULL) {
        return NULL;
    }

    size_t temp = number;
    size_t size = 0;

    while(temp > 0) {
        size_t res = temp%10;

        size++;

        str = realloc(str, size);
        if(str == NULL) {
            return NULL;
        }

        str[size-1] = res+'0';

        temp -= res;
        temp /= 10;
    }

    char temp_tab[size];
    strncpy(temp_tab, str, size);

    for(size_t i = 0; i < size; ++i) {

        str[i] = temp_tab[size -1 -i];
    }

    size++;
    str = realloc(str, size);
    str[size-1] = '\0';

    return str;
}
