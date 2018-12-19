#include "error.h"
#include "system.h"
#include "node_list.h"
#include "client.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "config.h"
#include "args.h"

#define MAX_PORT_NUMBER 65535
#define MAX_IP_SIZE 15
#define OCTET 8

int get_list_size(const char* msg);
void print_content(const kv_pair_t* list, const size_t list_size);
error_code add_in_list(kv_pair_t* list, const size_t from, const size_t to, int* counter, const char* in_msg);
void delete_list_content(kv_pair_t* list, const size_t size);

int main(int argc, char* argv[])
{

    int s = get_socket(1);
    if(s < 0 || argc < 3) {
        printf("FAIL\n");
        return -1;
    }



    char ip_address[MAX_IP_SIZE + 1];
    memset(ip_address, 0, MAX_IP_SIZE + 1);

    strcpy(ip_address, argv[1]);

    size_t port_size_t = 0;
    str_to_int(argv[2], &port_size_t);

    if(port_size_t > MAX_PORT_NUMBER) {

        printf("FAIL\n");
        return -1;
    }

    uint16_t port = (uint16_t) port_size_t;


    // Load server address
    struct sockaddr_in srv_addr;
    socklen_t addr_len = sizeof(srv_addr);

    error_code error = get_server_addr(ip_address, port, &srv_addr);
    if(error != ERR_NONE) {

        printf("FAIL\n");
        return -1;
    }

    char eof = '\0';

    if(-1 == sendto(s, &eof, 1, 0, (struct sockaddr*)&srv_addr, addr_len)) {	//send \0 to the server
        printf("FAIL\n");
        return -1;
    }

    char in_msg[MAX_MSG_SIZE];		//where the incoming messages will be temporarily stored
    memset(in_msg, '\0', MAX_MSG_SIZE);


    //Get the first message and extract the size of the hashtable content

    ssize_t in_msg_len = recvfrom(s, in_msg, MAX_MSG_SIZE, 0, (struct sockaddr*)&srv_addr, &addr_len);
    int list_size = 0;

    if(in_msg_len <= 0) {
        printf("FAIL\n");
        return -1;
    }


    list_size = get_list_size(in_msg);	//get back the size

    int counter = 0;			//counts how many pairs have been added

    kv_pair_t* list = calloc(list_size, sizeof(kv_pair_t));	//where we will store all kv_pairs

    if(list == NULL) {
        printf("FAIL\n");
        return -1;
    }

    error_code error_add_in_list = add_in_list(list, sizeof(int), in_msg_len, &counter, in_msg);	//add the content of the first message in the list

    if(error_add_in_list != ERR_NONE) {
        printf("FAIL\n");
        return -1;
    }

    //receive the other messages

    while(counter < list_size) {

        in_msg_len = recvfrom(s, in_msg, MAX_MSG_SIZE, 0, (struct sockaddr*)&srv_addr, &addr_len);

        if(in_msg_len <= 0) {
            printf("FAIL\n");
            return -1;
        }

        error_add_in_list = add_in_list(list, 0, in_msg_len, &counter, in_msg);	//add the content of the UDP message

        if(error_add_in_list != ERR_NONE) {
            printf("FAIL\n");
            return -1;
        }

    }

    print_content(list, list_size);

    delete_list_content(list, list_size);

    free(list);
    list = NULL;

    return 0;
}


void print_content(const kv_pair_t* list, const size_t list_size) 	//use for printing the result of the dump
{

    for(size_t i = 0; i < list_size; ++i) {

        printf("%s = %s\n", list[i].key, list[i].value);

    }
    fflush(stdout);
}


error_code add_in_list(kv_pair_t * list, const size_t from, const size_t to, int* counter, const char* in_msg)  // add in the list the content of the UDP messages, from is inclusive, to in exclusive, counter is the number of pairs in added in the list
{
    size_t sub_index = from;

    while(sub_index < to) {

        size_t key_size = 0;
        size_t value_size = 0;

        while(in_msg[sub_index + key_size] != '\0') {
            key_size++;
        }
        while(in_msg[sub_index + key_size + 1 + value_size ] != '\0') {
            value_size++;
        }

        char* key = calloc(key_size + 1, sizeof(char));
        char* value = calloc(value_size + 1, sizeof(char));

        if(key == NULL || value == NULL) {
            return ERR_NOMEM;
        }

        strncpy(key, in_msg + sub_index, key_size);
        key[key_size] = '\0';

        strncpy(value, in_msg + sub_index + 1 + key_size, value_size);
        value[value_size] = '\0';

        list[*counter].key = key;
        list[*counter].value = value;

        sub_index += key_size + 1 + value_size + 1;
        *counter += 1;
    }


    return ERR_NONE;
}


int get_list_size(const char* msg) 	//get the number of kv_pairs we will receive from the server
{
    int res = msg[0];

    for(size_t i = 1; i < sizeof(int); i++) {
        res = (res << OCTET) | msg[i];
    }
    return ntohl(res);
}

void delete_list_content(kv_pair_t* list, const size_t size)
{

    for(size_t i = 0; i < size; ++i) {

        free_const_ptr(list[i].key);
        free_const_ptr(list[i].value);
        list[i].key = NULL;
        list[i].value = NULL;

    }
}

