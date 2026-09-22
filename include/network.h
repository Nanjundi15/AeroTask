#ifndef AEROTASK_NETWORK_H
#define AEROTASK_NETWORK_H
int network_server_run(int port);
int network_client_run(const char *host, int port, int argc, char **argv);
#endif
