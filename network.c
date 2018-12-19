#include "network.h"
#include "ring.h"
#include <arpa/inet.h>
#include <unistd.h>

#define OCTET 8
#define MAX_VAL_OCTET 255
#define TIMEOUT 1
#define MAX_IP_SIZE 15

typedef enum {FAILURE, SUCCESS} server_feedback;


// those are helper functions, we don't put the prototypes in network.h
ssize_t send_get_to_node(const int socket, const node_t node,const struct sockaddr_in *srv_addr, const pps_key_t key);
ssize_t receive_put_from_node(const int socket, struct sockaddr *src_addr);
int receive_get_from_node(const int socket, pps_value_t *retrieved_value, struct sockaddr *src_addr);
ssize_t send_put_to_node(const int socket, const struct sockaddr_in *srv_addr, const pps_key_t key, const pps_value_t value);
void create_message(char * const array,const pps_key_t key,const pps_value_t value);
int is_src_valid(const node_list_t* n_list, const struct sockaddr* src_addr);
error_code update_internal_Htable(Htable_t* table, pps_value_t *value, server_feedback* request_state, client_t* client);



error_code network_get(client_t client, pps_key_t key, pps_value_t *value)
{
    if(value == NULL) {
        return ERR_BAD_PARAMETER;
    }
    M_EXIT_IF_TOO_LONG(key, MAX_MSG_ELEM_SIZE, "Key too long  in network.c");

    // Get client socket
    int op_socket = get_socket(TIMEOUT);
    if(op_socket == -1) {
        return ERR_NETWORK;
    }

    server_feedback failure = FAILURE;

    Htable_t table = construct_Htable(HTABLE_SIZE);	//htable used to count answers

    ring_t* ring = ring_alloc();

    error_code error = ring_init(ring);
    M_EXIT_IF_ERR(error, "ring_init error in network_get");

    node_list_t* n_list = ring_get_nodes_for_key(ring, client.options->N, key);


    //Send to nodes with helper function without caring about the answers
    for(size_t i = 0; i < client.options->N; i++) {
        node_t currNode = n_list->list[i];

        if(send_get_to_node(op_socket, currNode, &currNode.socket, key) != strlen(key)) {
            debug_print("ERR_NETWORK : could not send to node");
        }

    }


    int exit_for_timeout = 0;
    struct sockaddr src_addr;

    while(failure != SUCCESS && exit_for_timeout == 0) {
        //Receive response
        if(receive_get_from_node(op_socket, value, &src_addr) != 0) {
            exit_for_timeout = 1;
            debug_print("ERR_NETWORK : could not receive from node");
        } else { //no network error
            if(*value != NULL && is_src_valid(n_list, &src_addr) == 1) {
                update_internal_Htable(&table, value, &failure, &client);
            }
        }
    }

    close(op_socket);
    delete_Htable_and_content(&table);

    M_EXIT_IF(failure != SUCCESS, ERR_NETWORK, "Network Error", , );

    return ERR_NONE;
}

error_code update_internal_Htable(Htable_t* table, pps_value_t *value, server_feedback* request_state, client_t* client)
{

    char * temp = malloc(sizeof(char));
    if(temp == NULL) {
        delete_Htable_and_content(table);
        return ERR_NOMEM;
    }


    if(get_Htable_value(*table, *value) == NULL) {	//if it's the first time we get that particular value
        temp[0] = '1';
    } else {	//else increment counter

        temp[0] = get_Htable_value(*table, *value)[0] + 1;
    }

    pps_value_t new_value = temp;

    //temporary key is needed because add_Htable_value will free it
    char* tempValueCpy = calloc(strlen(*value), sizeof(char));
    strncpy(tempValueCpy, *value, strlen(*value));
    pps_key_t temp_key  = tempValueCpy;

    error_code err = add_Htable_value(*table, temp_key, new_value);	//add the updated count in the hashtable

    if(err != ERR_NONE) {
        delete_Htable_and_content(table);
        return err;
    }

    if((size_t)(get_Htable_value(*table, *value)[0] - '0') >= client->options->R) {	//return the value when the quorum is reached
        *request_state = SUCCESS;
        //save the value to be returned before the table gets REKKT
        *value = strdup(*value);

    }

    return ERR_NONE;

}



ssize_t send_get_to_node(const int socket, const node_t node, const struct sockaddr_in *srv_addr, const pps_key_t key)
{
    // Send message
    return sendto(socket, key, strlen(key), 0, (struct sockaddr*)srv_addr, sizeof(*srv_addr));
}


error_code network_put(client_t client, pps_key_t key, pps_value_t value)
{

    M_EXIT_IF_TOO_LONG(key, MAX_MSG_ELEM_SIZE, "Key too long  in network.c");
    M_EXIT_IF_TOO_LONG(value, MAX_MSG_ELEM_SIZE, "Value too long  in network.c");

    int op_socket = get_socket(TIMEOUT);
    if(op_socket == -1) {
        return ERR_NETWORK;
    }


    ring_t* ring = ring_alloc();

    error_code error = ring_init(ring);
    M_EXIT_IF_ERR(error, "ring_init error in network_get");

    node_list_t* n_list = ring_get_nodes_for_key(ring, client.options->N, key);

    //Send  N messages without caring about the answer
    for(size_t i = 0; i < client.options->N; i++) {

        node_t currNode = n_list->list[i];

        // Send to node with helper function
        if(send_put_to_node(op_socket, &currNode.socket, key, value) != strlen(key)+strlen(value)+1) {
            debug_print("ERR_NETWORK : could not send to node");
        }
    }

    struct sockaddr src_addr;
    int positive_ans = 0;

    while(positive_ans < client.options->W) {
        //Receive response

        //if timeout while not enough answers have arrived => ERR_NETWORK
        if(receive_put_from_node(op_socket, &src_addr) == -1) {
            close(op_socket);
            return ERR_NETWORK;
        } else {

            if(is_src_valid(n_list, &src_addr) == 1) {
                positive_ans++;
            }

        }
    }
    close(op_socket);

    return ERR_NONE;
}



ssize_t send_put_to_node(const int socket, const struct sockaddr_in *srv_addr, const pps_key_t key, const pps_value_t value)
{
    //Prepare message
    size_t size = strlen(key) + strlen(value) + 1;
    char message[size];
    for(size_t i = 0; i < size; ++i) {

        if(i < strlen(key)) {
            message[i] = key[i];
        } else if(i == strlen(key)) {
            message[i] = '\0';
        } else {
            message[i] = value[i - strlen(key) -1];
        }
    }

    // Send message
    ssize_t res = sendto(socket, &message, size, 0, (struct sockaddr*)srv_addr, sizeof(*srv_addr));

    return res;
}


int receive_get_from_node(const int socket, pps_value_t *retrieved_value, struct sockaddr *src_addr)
{

    char in_msg[MAX_MSG_ELEM_SIZE];
    const char* eof = calloc(1, sizeof(char));

    socklen_t addrlen = sizeof(*src_addr);

    ssize_t in_msg_len = recvfrom(socket, &in_msg, sizeof(in_msg), 0, src_addr, &addrlen);

    if(in_msg_len >= 0 && in_msg_len <= MAX_MSG_ELEM_SIZE) { // Valid response

        if(in_msg_len == 0) { //response from a get with a known key but value is '\0'
            *retrieved_value = eof;
        } else if(in_msg_len == 1 && in_msg[0] == '\0') { //response from a get with an unknown key
            *retrieved_value = NULL;
        } else {

            char* temp = calloc(in_msg_len, sizeof(char));	//"normal" response from GET    we have to free the memory in pps-client
            strncpy(temp,in_msg, in_msg_len);
            *retrieved_value = (pps_value_t)temp;
        }

        return 0;
    }

    //Timeout or wrong msg size or error from PUT
    return 1;
}

ssize_t receive_put_from_node(const int socket, struct sockaddr *src_addr)
{

    char in_msg[MAX_MSG_ELEM_SIZE];
    socklen_t addrlen = sizeof(*src_addr);

    return recvfrom(socket, &in_msg, MAX_MSG_ELEM_SIZE, 0, src_addr, &addrlen);

}


int is_src_valid(const node_list_t* n_list, const struct sockaddr* src_addr)
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
            if(((struct sockaddr_in*)src_addr)->sin_port == currNode.socket.sin_port && strcmp(ip1, ip2) == 0) { //the message is from this particular node (valid node in the list)
                return 1;
            }
        }
    }


    return 0;
}
