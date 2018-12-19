#include "network.h"
#include "client.h"
#include "node.h"
#include "system.h"
#include <time.h>
#include <sys/sysinfo.h>

int main(int argc, char* argv[])
{
    //initialize client
    client_t client;
    client_init_args_t cl_init;
    cl_init.client = &client;
    cl_init.argv = &argv;
    cl_init.required_args = 2;
    cl_init.supported_args = (TOTAL_SERVERS | PUT_NEEDED);
    cl_init.argc = argc;

    error_code err = client_init(cl_init);

    //get key and value from argv
    pps_key_t key = argv[0];
    pps_value_t value = argv[1];

    if(err != ERR_NONE) {
        printf("FAIL\n");
        return -1; // error in main
    }

#ifdef PERF_TEST

    // PERFORMANCE MESURE BEGIN
    struct timespec time_start, time_end;

    int res_start = 0;

    do {
        res_start = clock_gettime(CLOCK_MONOTONIC, &time_start);
    } while(res_start != 0);

#endif

    error_code res = network_put(client, key, value);	//send data and wait for ACK

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

    if(res == ERR_NONE) {	//feedback
        printf("OK\n");
    } else {
        printf("FAIL\n");
    }

    client_end(&client);

    return 0;
}
