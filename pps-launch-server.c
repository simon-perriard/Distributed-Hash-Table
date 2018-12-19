#include "system.h"
#include "client.h"
#include "node.h"
#include "network.h"
#include "error.h"
#include "hashtable.h"


// standard includes (printf, exit, ...)
#include <stdio.h>
#include <stdlib.h>
#include <errno.h> // for errno
#include <string.h> // for memset()
#include <math.h>

// for basic socket communication
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


#define OCTET 8
#define MAX_VAL_OCTET 255
#define MAX_PORT_NUMBER 65535
#define MAX_IP_SIZE 15

void get_request(const int socket, const pps_key_t key, const struct sockaddr_in *cli_addr, const Htable_t table);
void put_request(const int socket, kv_pair_t pair, const struct sockaddr_in *cli_addr, Htable_t table);
void construct_pair(const char *input, kv_pair_t * const pair, const size_t size);
void get_IP_PORT(char* const ip_address, uint16_t* const port);
void dump_answer(const int socket, const struct sockaddr_in *cli_addr, const Htable_t table);
void add_in_msg(char* const message, const char* input, const size_t size, size_t* const start_index);
void add_size_in_msg(char* const msg, const size_t nb_pairs);
int get_nbr_pairs(const Htable_t table);
int check_EOF(const char* s, size_t size_to_check);

int main(void)
{

    Htable_t table;	//creation of the hashtable
    table = construct_Htable(HTABLE_SIZE);

    // Set up socket (without timeout, for servers).
    int s = get_socket(0);
    if(-1 == s) {
        debug_print("unable to get a socket in pps-launch-server.c");
        return -1;
    }

    char* ip_address = calloc(MAX_IP_SIZE + 1, sizeof(char));
    uint16_t port = 0;
    get_IP_PORT(ip_address, &port);

    // Bind server to the address:port
    error_code err = bind_server(s, ip_address, port);
    M_EXIT_IF_ERR(err,"unable to bind ip with port in pps-launch-server.c");

    // Receive messages forever.
    while (1) {
        // Receive message and get return address.
        struct sockaddr_in cli_addr;
        socklen_t addr_len = sizeof(cli_addr);
        memset(&cli_addr, 0, addr_len); //fill cli_addr with 0's

        char in_msg[MAX_MSG_SIZE]; // have enough place to have either a get or a put
        memset(in_msg,'\0',MAX_MSG_SIZE);

        ssize_t in_msg_len = recvfrom(s, in_msg, MAX_MSG_SIZE, 0, (struct sockaddr *) &cli_addr, &addr_len);

        if(in_msg_len >= 0) {
            if(0 == in_msg_len) {	//answer the "ping" from pps-list-nodes  with an empty datagram

                sendto(s, NULL, 0, 0, (struct sockaddr*)&cli_addr, addr_len);

            } else if(1 == in_msg_len && !check_EOF(in_msg, in_msg_len)) {	//answer to the dump command

                dump_answer(s, &cli_addr, table);
            } else if(check_EOF(in_msg, in_msg_len)) { // that will be a get request => no /0  in message

                pps_key_t key = in_msg;
                get_request(s, key, &cli_addr, table);

            } else { // that will be a put request
                kv_pair_t pair;
                construct_pair(in_msg, &pair, in_msg_len);
                put_request(s, pair, &cli_addr, table);

            }
        }
    }

    free(ip_address);			//never used because the server is stopped with ctrl C
    ip_address = NULL;
    delete_Htable_and_content(&table);
}

void construct_pair(const char* input, kv_pair_t * const pair, size_t size)
{

    char* end = strchr(input, '\0');
    size_t key_size =  end - input;

    size_t value_size = size - key_size -1;

    char* k = NULL;
    char* v = NULL;

    k = calloc(key_size, sizeof(char));
    v = calloc(value_size, sizeof(char));

    if(k != NULL && v != NULL) {
        strncpy(k,input, key_size);
        strncpy(v,input + key_size + 1, value_size);
        pair->key = k;
        pair->value = v;
    } else if(k == NULL) {
        free(v);
        v = NULL;
    } else {
        free(k);
        k = NULL;
    }

}


void get_request(const int socket, const pps_key_t key, const struct sockaddr_in *cli_addr, const Htable_t table)
{
    pps_value_t value = get_Htable_value(table, key);

    if(value == NULL) {	//the key wasn't found in the hashtable
        char eof = '\0';
        sendto(socket, &eof, 1, 0, (struct sockaddr*)cli_addr, (socklen_t)sizeof(*cli_addr));		//send \0
    } else {
        sendto(socket, value, strlen(value), 0, (struct sockaddr*)cli_addr, (socklen_t)sizeof(*cli_addr));
    }
}


void put_request(const int socket, kv_pair_t pair, const struct sockaddr_in *cli_addr, Htable_t table)
{

    error_code res = add_Htable_value(table, pair.key, pair.value);

    if(res == ERR_NONE) { // empty datagram for ACK
        sendto(socket, NULL, 0, 0, (struct sockaddr*)cli_addr, (socklen_t)sizeof(*cli_addr));
    }
}

void get_IP_PORT(char* ip_address, uint16_t* port)
{

    int match = 0;

    do {

        printf("IP port? ");
        fflush(stdout);

        match = fscanf(stdin, "%15s %hu", ip_address, port); //read the ip and the port number
    } while(match != 2 || *port <= 0 || *port > MAX_PORT_NUMBER);
}

void dump_answer(const int socket, const struct sockaddr_in *cli_addr, const Htable_t table)
{

    size_t actual_msg_size = 0; //number of bytes
    char msg[MAX_MSG_SIZE];
    memset(msg, '\0',MAX_MSG_SIZE);

    int nbr_pairs = get_nbr_pairs(table);

    nbr_pairs = htonl(nbr_pairs);

    add_size_in_msg(msg, nbr_pairs);
    actual_msg_size += sizeof(int); //we added the size


    int first_msg = 1;

    const char eof = '\0';

    for(size_t i = 0; i < table.size; ++i) {

        bucket_t b = table.list[i];

        for(size_t j = 0; j < b.size; ++j) {

            kv_pair_t curr_pair = b.pairs[j];
            pps_key_t key = curr_pair.key;
            pps_value_t value = curr_pair.value;

            if(actual_msg_size + strlen(key) + strlen(value) + 2 > MAX_MSG_SIZE) { //check if the pair that will be added can fit in the UDP message
                fflush(stdout);
                sendto(socket, msg, actual_msg_size, 0, (struct sockaddr*)cli_addr, (socklen_t)sizeof(*cli_addr));
                actual_msg_size = 0;
                memset(msg, '\0', MAX_MSG_SIZE);
                first_msg = 1;
            }

            if(first_msg == 0) { //only put '\0' before value if the pair is not at the beginning of the message
                fflush(stdout);
                add_in_msg(msg, &eof, 1, &actual_msg_size);

            }

            fflush(stdout);
            add_in_msg(msg, key, strlen(key), &actual_msg_size);
            add_in_msg(msg, &eof, 1, &actual_msg_size);
            add_in_msg(msg, value, strlen(value), &actual_msg_size);

            first_msg = 0;
        }
    }


    //if the buffer is not empty and we reached the end of the table, send the remaining elements
    if(actual_msg_size != 0) {
        fflush(stdout);
        sendto(socket, msg, actual_msg_size, 0, (struct sockaddr*)cli_addr, (socklen_t)sizeof(*cli_addr));
    }

}

void add_in_msg(char* const message, const char* input, const size_t size, size_t* const start_index)
{

    for(size_t i = 0; i < size; i++) {
        message[*start_index + i] = input[i];
    }
    *start_index += size;
}

int get_nbr_pairs(const Htable_t table)
{
    int res = 0;

    for(size_t i = 0; i < table.size; i++) {
        res += table.list[i].size;
    }
    return res;
}

void add_size_in_msg(char* const msg, const size_t nb_pairs)
{

    for(size_t i = 0; i < sizeof(int); i++) {
        msg[i] = (nb_pairs >> (sizeof(int)-1-i)*OCTET) & MAX_VAL_OCTET;
    }
}


int check_EOF(const char* s, size_t size_to_check)
{

    for(size_t i = 0; i < size_to_check; ++i) {

        if(s[i] == '\0') {

            return 0;
        }

    }
    return 1;
}
