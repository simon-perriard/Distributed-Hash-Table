#include "network.h"
#include "client.h"
#include "node.h"
#include "system.h"
#include <math.h>

int check_index(size_t length, int index, size_t size);

int main(int argc, char* argv[])
{

    size_t index_u = 0;
    int index = 0;
    size_t length = 0;

    //initialize client
    client_t client;
    client_init_args_t cl_init;
    cl_init.client = &client;
    cl_init.argv = &argv;
    cl_init.required_args = 4;
    cl_init.supported_args = (TOTAL_SERVERS | GET_NEEDED | PUT_NEEDED);
    cl_init.argc = argc;

    error_code err = client_init(cl_init);

    if(err != ERR_NONE) {
        printf("FAIL\n");
        return -1; // error in main
    }

    pps_key_t src = argv[0];
    pps_key_t dst = argv[3];

    if((isNaN(argv[1]) && (argv[1][0] != '-' || isNaN(argv[1] + 1) )) || isNaN(argv[2])) { //TOP-NOTCH BOOLEAN ALGEBRA
        printf("FAIL\n");
        return -1;
    }


    str_to_int(argv[2], &length);

    if(argv[1][0] == '-') {
        str_to_int(argv[1] + 1, &index_u);
        index = - index_u;
    } else {
        str_to_int(argv[1], &index_u);
        index = index_u;
    }

    pps_value_t src_value = NULL;


    error_code res_get = network_get(client, src, &src_value);
    if(res_get != ERR_NONE) {
        printf("FAIL\n");
        return -1;
    }

    size_t size = strlen(src_value);

    if(check_index(length, index, size)) {
        printf("FAIL\n");
        return -1;
    }


    //substring + put

    char* sub_string = calloc(length, sizeof(char));
    if(sub_string == NULL) {
        printf("FAIL\n");
        return -1;
    }

    if(index >= 0) {
        strncpy(sub_string, src_value + index, length);
    } else {
        strncpy(sub_string, src_value + size + index, length);
    }


    error_code res_put = network_put(client, dst, (pps_value_t)sub_string);

    if(res_put != ERR_NONE) {
        printf("FAIL\n");
        return -1;
    }
    free(sub_string);
    sub_string = NULL;

    printf("OK\n");
    client_end(&client);
    return 0;
}


int check_index(size_t length, int index, size_t size)
{

    if(index < 0 && ( -index < 1 || -index > size || -index - length + 1 < 1)) {
        return 1;
    } else if(index >= 0 && ( index < 0 || index >= size || index + length - 1 >= size)) {
        return 1;
    }

    return 0;
}

