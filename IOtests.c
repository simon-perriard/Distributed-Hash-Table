#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>


int main(void)
{

    FILE* f = fopen("servers.txt", "r");
    char* string = calloc(16,sizeof(char));
    uint16_t port = 0;

    int match = 0;

    do {



        //if(!feof(f)){
        match = fscanf(f,"%15s %hu",string,&port);

        if(port <= 0 || port > 3254324 ) {
            fclose(f);
            free(string);
            return -1;
        }

        printf("%d ",match);
        printf("%s ",string);
        printf("%d\n",port);

        //}


    } while(!feof(f));

    free(string);
    fclose(f);
    return 0;
}
