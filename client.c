#include "client.h"


size_t min(size_t x, size_t y);


size_t count_args(char*** args);

void client_end(client_t* client)
{
    free(client->options);
    client->options = NULL;
}

error_code client_init(client_init_args_t args)
{
    client_t* client = args.client;
    const char* name = (*args.argv)[0];

    M_EXIT_IF(args.argc < args.required_args, ERR_BAD_PARAMETER, "Bad parameters",,);

    client->options = parse_opt_args(args.supported_args, args.argv);	//parse the options and "shift" argv

    //check whether there is enough remaining arguments for a given shell command
    M_EXIT_IF(count_args(args.argv) < args.required_args, ERR_BAD_PARAMETER, "Bad parameters",,);

    M_EXIT_IF(client->options == NULL, ERR_BAD_PARAMETER, "Bad param",,);

    client->l_node = get_nodes();	//get the list of all nodes in server.txt

    client->options->N = min(client->l_node->nbre_of_servers, client->options->N);	//set the options with a few verifications

    client->options->R = min(client->options->N, client->options->R);
    client->options->W = min(client->options->N, client->options->W);

    M_EXIT_IF(client->l_node == NULL, ERR_IO, "errors occured in  node_list.c",,);

    client->name = name;

    return ERR_NONE;

}

size_t count_args(char*** args)	//count the number of arguments in argv
{

    size_t size = 0;

    while((*args)[size] != NULL) {
        size++;
    }

    return size;

}

size_t min(size_t x, size_t y)	//min function
{
    return x < y ? x : y;
}
