#include "node_list.h"
#define MAX_PORT_NUMBER 65535
#define MAX_IP_SIZE 15

node_list_t* put_in_list(node_list_t* new_list, char* ip_address, uint16_t port_number, size_t id);

node_list_t* node_list_new()
{

    node_list_t* new_list = malloc(sizeof(node_list_t));	//allocate memory
    if(new_list == NULL) {
        return NULL;
    }

    new_list->list = malloc(0);		//the list will be reallocated later when we will add the nodes
    new_list->size = 0;
    new_list->nbre_of_servers = 0;

    return new_list;
}


node_list_t* get_nodes()
{

    node_list_t* new_list = node_list_new();

    if(new_list == NULL) {
        return NULL;
    }


    FILE* f_stream = fopen(PPS_SERVERS_LIST_FILENAME, "r");		//read nodes from file
    if(f_stream == NULL) {
        return NULL;
    }

    int match = 0;

    do {
        uint16_t port_number = 0;
        int id = 0;
        char ip_address[MAX_IP_SIZE + 1]; //allocate memory for the ip + \0

        if(ip_address == NULL) {
            fclose(f_stream);
            return NULL;
        }

        match = fscanf(f_stream,"%15s %hu %d", ip_address, &port_number, &id);

        if(3 == match && port_number > 0 && port_number <= MAX_PORT_NUMBER && id > 0) { //good input

            new_list = put_in_list(new_list, ip_address, port_number, (size_t)id);

            if(new_list == NULL) {

                fclose(f_stream);
                return NULL;
            }

            new_list->nbre_of_servers++;

        } else {

            if(match != -1) { //bad input
                fclose(f_stream);
                return NULL;

            } else { //we reached the end of the file. Error due to ip will return a NULL later in the execution

                fclose(f_stream);

                return new_list;
            }
        }


    } while(!feof(f_stream));

    fclose(f_stream);

    return new_list;
}


node_list_t* put_in_list(node_list_t* new_list, char* ip_address, uint16_t port_number, size_t id)
{
    for(size_t i = 1; i <= id; i++) {

        node_t new_node;	//creation of new node

        char* ip_addr = calloc(MAX_IP_SIZE + 1, sizeof(char)); //allocate memory for the ip + \0
        if(ip_addr == NULL) {

            return NULL;
        }

        strncpy(ip_addr, ip_address, MAX_IP_SIZE + 1);

        error_code err = node_init(&new_node, ip_addr, port_number, i);	//initialize the node

        if(err != ERR_NONE) {
            return NULL;
        }

        if(node_list_add(new_list, new_node) != ERR_NONE) {		//add the node to the list
            return NULL;
        }
    }

    return  new_list;
}


void node_list_free(node_list_t *list)
{

    for(int i = 0; i < list->size; ++i) {

        node_end(&list->list[i]);		//fin de vie des serveurs
    }

    free(list->list);
    list->list = NULL;
    free(list);
    list = NULL;
}


error_code node_list_add(node_list_t *n_list, node_t node)
{

    M_REQUIRE_NON_NULL(n_list);

    n_list->size += 1;
    size_t new_internal_size = n_list->size * sizeof(node_t);
    n_list->list = realloc(n_list->list, new_internal_size);		//re-allocate memory for the new element

    M_EXIT_IF(n_list == NULL,ERR_NOMEM,"Not enough memory  in  node_list.c",,);

    size_t index_to_add = n_list->size - 1;

    n_list->list[index_to_add] = node;

    return ERR_NONE;
}


void node_list_sort(node_list_t *list, int (*comparator)(const node_t *, const node_t *))
{
    qsort(list->list, list->size, sizeof(node_t), (int(*)(const void*,const void*))comparator);
}
