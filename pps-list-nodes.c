#include "error.h"
#include "system.h"
#include "node_list.h"
#include "client.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>


#define MAX_IP_SIZE 15
#define TIMEOUT 1

void print_sha(unsigned char sha_1[]);
int set_contains(const struct sockaddr_in* sin_set, size_t* set_size, const struct sockaddr_in* sa);
void add_in_set(struct sockaddr_in* sin_set, size_t* set_size, const struct sockaddr* sa);
int src_valid(const node_list_t* n_list, const struct sockaddr* src_addr);

int main(int argc, char* argv[])
{
    //initialize client
    client_t client;
    client_init_args_t cl_init;
    cl_init.client = &client;
    cl_init.argv = &argv;
    cl_init.required_args = 0;
    cl_init.supported_args = 0;
    cl_init.argc = argc;

    error_code err = client_init(cl_init);


    if(err != ERR_NONE) {
        printf("FAIL\n");
        return -1; // error in main
    }


    int op_socket = get_socket(TIMEOUT);
    if(op_socket == -1) {
        printf("FAIL\n");
        return -1;
    }

    //send to all the nodes
    for(size_t i = 0; i < client.l_node->size; i++) {
        node_t curr_node = client.l_node->list[i];
        if(-1 == sendto(op_socket, NULL, 0, 0, (struct sockaddr*)&curr_node.socket, sizeof(curr_node.socket))) {
            printf("FAIL to send\n");
        }
    }

    //set in which the info about the responding servers will be held
    struct sockaddr_in result_set[client.l_node->nbre_of_servers];
    memset(result_set, 0, sizeof(struct sockaddr_in));

    size_t set_size = 0;

    int nb_ans = 0;
    int exit_for_timeout = 0;

    struct sockaddr src_addr;
    socklen_t addrlen = sizeof(src_addr);

    while(nb_ans < client.l_node->size && exit_for_timeout == 0) {
        char in_msg[MAX_MSG_SIZE];

        ssize_t in_msg_len = recvfrom(op_socket, in_msg, MAX_MSG_SIZE, 0, &src_addr, &addrlen);

        if(0 == in_msg_len && src_valid(client.l_node, &src_addr)) {
            add_in_set(result_set, &set_size, &src_addr);
            nb_ans++;
        } else if(-1 == in_msg_len) {
            exit_for_timeout = 1;
        }

    }

    node_list_sort(client.l_node, node_cmp_server_addr);

    char ip[MAX_IP_SIZE+1];

    for(size_t i = 0; i < client.l_node->size; i++) {

        node_t currNode = client.l_node->list[i];

        inet_ntop(AF_INET, &(currNode.socket.sin_addr), ip, MAX_IP_SIZE+1);
        printf("%s %hu ", ip, ntohs(currNode.socket.sin_port));
        print_sha(currNode.sha1);

        if(set_contains(result_set, &set_size, &currNode.socket) == 1) {
            printf("OK\n");
        } else {
            printf("FAIL\n");
        }
    }

    close(op_socket);
    client_end(&client);

    return 0;
}

void print_sha(unsigned char sha_1[])
{
    if(sha_1 != NULL) {
        printf("(");
        for(int i = 0; i < SHA_DIGEST_LENGTH; i++) {
            printf("%02x", sha_1[i]);
        }
        printf(") ");
    }
}

int set_contains(const struct sockaddr_in* sin_set, size_t* set_size, const struct sockaddr_in* sa)
{

    for(size_t i = 0; i < *set_size; i++) {

        //check ports
        if(sa->sin_port == sin_set[i].sin_port) {
            char ip1[MAX_IP_SIZE+1];
            char ip2[MAX_IP_SIZE+1];

            //put back the IP addresses in order to be compared
            inet_ntop(AF_INET, &(sin_set[i].sin_addr), ip1, MAX_IP_SIZE+1);
            inet_ntop(AF_INET, &(sa->sin_addr), ip2, MAX_IP_SIZE+1);

            //return 1 if ports and IPs both match
            if(strcmp(ip1, ip2) == 0) {
                return 1;
            }
        }
    }
    // 0 if the set does not contain sa
    return 0;
}

void add_in_set(struct sockaddr_in* sin_set, size_t* set_size, const struct sockaddr* sa)
{

    //add only if sa is not already in sin_set
    if(sa->sa_family == AF_INET && set_contains(sin_set, set_size, (struct sockaddr_in*)sa) == 0) {
        sin_set[*set_size] = *(struct sockaddr_in*)sa;
        *set_size +=1 ;
    }
}


int src_valid(const node_list_t* n_list, const struct sockaddr* src_addr)
{
    for(size_t i = 0; i < n_list->size; i++) {
        //sa_family should be AF_INET

        if(src_addr->sa_family == AF_INET) {

            node_t currNode = n_list->list[i];

            char ip1[MAX_IP_SIZE+1];
            char ip2[MAX_IP_SIZE+1];

            //put back the IP addresses in order to be compared
            inet_ntop(AF_INET, &currNode.socket.sin_addr, ip1, MAX_IP_SIZE+1);
            inet_ntop(AF_INET, &(((struct sockaddr_in*)src_addr)->sin_addr), ip2, MAX_IP_SIZE+1);

            //return 1 if ports and IPs both match
            if(((struct sockaddr_in*)src_addr)->sin_port == currNode.socket.sin_port && strcmp(ip1, ip2) == 0) {
                return 1;
            }
        }
    }


    return 0;
}
