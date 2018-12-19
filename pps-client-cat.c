#include "network.h"
#include "client.h"
#include "node.h"
#include "system.h"


int main(int argc, char* argv[])
{
    //initialize client
    client_t client;
    client_init_args_t cl_init;
    cl_init.client = &client;
    cl_init.argv = &argv;
    cl_init.required_args = 2;
    cl_init.supported_args = (TOTAL_SERVERS | GET_NEEDED | PUT_NEEDED);
    cl_init.argc = argc;

    error_code err = client_init(cl_init);

    if(err != ERR_NONE) {
        printf("FAIL\n");
        return -1; // error in main
    }


    char* concatBuffer = malloc(MAX_MSG_ELEM_SIZE); //we will concat in this (buffer) array
    if(concatBuffer == NULL) {
        printf("FAIL\n");
        return -1;
    }
    memset(concatBuffer, '\0', MAX_MSG_ELEM_SIZE);

    pps_value_t valueBuffer = NULL;
    size_t i = 0;

    while(argv[i+1] != NULL) {//get all the values needed for concat

        error_code res = network_get(client, argv[i], &valueBuffer);
        if(res != ERR_NONE) {
            free(concatBuffer);
            printf("FAIL\n");
            return -1;
        }

        if(strlen(concatBuffer) + strlen(valueBuffer) < MAX_MSG_ELEM_SIZE) {
            strcat(concatBuffer, valueBuffer);//concat in the buffer
        } else {
            free(concatBuffer);
            printf("FAIL\n");
            return -1;
        }
        i++;
    }


    error_code res = network_put(client, argv[i], concatBuffer);//put the result in the DHT
    if(res != ERR_NONE) {
        free(concatBuffer);
        printf("FAIL\n");
        return -1;
    }


    printf("OK\n");
    free(concatBuffer);
    client_end(&client);
    return 0;
}
