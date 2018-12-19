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

    pps_key_t key1 = argv[0];
    pps_key_t key2 = argv[1];

    pps_value_t value1 = NULL;

    pps_value_t value2 = NULL;


    error_code res1 = network_get(client, key1, &value1);
    error_code res2 = network_get(client, key2, &value2);

    if(res1 != ERR_NONE || res2 != ERR_NONE) {
        printf("FAIL\n");
        return -1;
    } else {
        char* ptrRes = strstr(value1, value2);

        if(ptrRes == NULL) {
            printf("OK -1\n");
        } else {
            ptrdiff_t diff = (ptrRes - value1) / sizeof(char);
            printf("OK %td\n", diff);
        }
    }

    client_end(&client);

    return 0;
}
