#include "system.h"
#include "client.h"
#include "node.h"
#include "network.h"
#include <stdio.h>
#include "error.h"
#include "util.h"
#include <time.h>
#include <sys/sysinfo.h>


int main(int argc, char* argv[])
{


    //initialize client
    client_t client;
    client_init_args_t cl_init;
    cl_init.client = &client;
    cl_init.argv = &argv;
    cl_init.required_args = 1;
    cl_init.supported_args = (TOTAL_SERVERS | GET_NEEDED);
    cl_init.argc = argc;

    error_code err = client_init(cl_init);

    if(err != ERR_NONE) {
        printf("FAIL\n");
        return -1; // error in main
    }


    pps_key_t key = argv[0];
    pps_value_t returnedValue = NULL;


#ifdef PERF_TEST

    // PERFORMANCE MESURE BEGIN
    struct timespec time_start, time_end;

    int res_start = 0;

    do {
        res_start = clock_gettime(CLOCK_MONOTONIC, &time_start);
    } while(res_start != 0);

#endif

    error_code res = network_get(client, key, &returnedValue);	//receive answer (value is allocated in network_get() => have to free later)

#ifdef PERF_TEST

    int res_end = 0;

    do {
        res_end = clock_gettime(CLOCK_MONOTONIC, &time_end);
    } while(res_end != 0);


    long nsec = time_end.tv_nsec - time_start.tv_nsec;
    while (nsec < 0) nsec += 1000000000;
    printf("%ld.%09ld\n", time_end.tv_sec - time_start.tv_sec, nsec);


    // PERFORMANCE MESURE END


#endif

    if(res == ERR_NONE) {
        printf("OK %s\n", returnedValue);
    } else {
        printf("FAIL\n");
    }
    free_const_ptr(returnedValue);		//free memory

    client_end(&client);
    return 0;

}
