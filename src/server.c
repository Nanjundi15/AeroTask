#include "network.h"
#include <stdio.h>
#include <stdlib.h>
int main(int argc,char **argv){
    int port=(argc>1)?atoi(argv[1]):8080;
    printf("Starting AeroTask server on port %d...\n",port);
    return network_server_run(port);
}
