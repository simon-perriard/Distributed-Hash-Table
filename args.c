#include "args.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>


size_t searchOption(const size_t supported_args, char* option, char ***rem_argv);


int isValidOption(const size_t supported_args, const char* str);


args_t *parse_opt_args(size_t supported_args, char ***rem_argv)
{
    args_t* args = malloc(sizeof(args_t));

    if(args == NULL) {
        return NULL;
    }

    args->N = 3;		//default values
    args->R = 2;
    args->W = 2;

    size_t index = 0; //index of the associated value
    size_t counter = 1; // there is always at least the programm's name


    if((supported_args & TOTAL_SERVERS)) {						//search for -n and update N in args
        index = searchOption(supported_args, "-n", rem_argv);
        if(index == 1) {
            return NULL;
        } else if(index != 0) {
            str_to_int((*rem_argv)[index], &args->N);
            counter += 2;
        }
    }

    if(supported_args & GET_NEEDED ) {							//search for -r and update R in args
        index = searchOption(supported_args, "-r", rem_argv);
        if(index == 1) {
            return NULL;
        } else if(index != 0) {
            str_to_int((*rem_argv)[index], &args->R);
            counter += 2;
        }
    }

    if((supported_args & PUT_NEEDED) ) {
        index = searchOption(supported_args, "-w", rem_argv);	//search for -w and update W in args
        if(index == 1) {
            return NULL;
        } else if(index != 0) {
            str_to_int((*rem_argv)[index], &args->W);
            counter += 2;
        }
    }

    if(((*rem_argv)[counter]!= NULL) && (strcmp((*rem_argv)[counter], "--") == 0)) {
        counter++;
    }

    *rem_argv += counter; // déplacer le pointeur afin que *rem_argv soit le premier élément qui n'est pas une option

    return args;

}


size_t searchOption(const size_t supported_args, char* option, char ***rem_argv)
{

    size_t index = 1;

    while((*rem_argv)[index] != NULL && strcmp("--", (*rem_argv)[index])) {

        if(!isValidOption(supported_args, (*rem_argv)[index]) && !(!isNaN((*rem_argv)[index]) && isValidOption(supported_args, (*rem_argv)[index -1]))) {
            return 0;
        }


        if(!strcmp(option, (*rem_argv)[index])) {
            if(((*rem_argv)[index + 1] == NULL) || isNaN((*rem_argv)[index + 1])) { // if associated value doesn't exist or is not an int, return 1
                return 1;
            } else {
                return index+1;
            }
        }

        index++;
    }

    //si pas trouvé l'option, retourne 0
    return 0;

}

int isNaN(const char* str)	//check if string is not a number
{

    for(size_t i = 0; i < strlen(str); i++) {
        if(!isdigit(str[i])) {
            return 1;
        }
    }

    return 0;

}


int isValidOption(const size_t supported_args, const char* str)	//check if the string is a valid option (n, r or w)
{
    if((supported_args & TOTAL_SERVERS) && !strcmp("-n", str)) {
        return 1;
    }

    if((supported_args & GET_NEEDED) && !strcmp("-r", str)) {
        return 1;
    }

    if((supported_args & PUT_NEEDED) && !strcmp("-w", str)) {
        return 1;
    }

    return 0;
}


void str_to_int(char* str, size_t* i)	//convert a string to an integer
{
    *i = 0;

    for(int j = strlen(str); j > 0; --j) {

        *i += (str[j-1] - '0')*pow(10, strlen(str) - j);	// - '0' to convert from ascii to integer
    }

}
