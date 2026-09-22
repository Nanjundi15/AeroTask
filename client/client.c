#include "network.h"
#include <stdio.h>
#include <stdlib.h>
int main(int argc,char **argv){
    if(argc<4){
        fprintf(stderr,"Usage: aerotask-client <host> <port> <command> [args...]\n");
        return 1;
    }
    return network_client_run(argv[1],atoi(argv[2]),argc-3,&argv[3]);
}
