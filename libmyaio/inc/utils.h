#ifndef MY_LIB_UILS_H
#define MY_LIB_UILS_H

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <sys/poll.h>
#include <pthread.h>
#include <arpa/inet.h>

int utils_accept_new_client(int srv_socket, struct pollfd *fds, struct sockaddr_in *cln_addr, int clientnum);
int utils_interact_with_client(int client_id, struct pollfd *fds, int *clientnum, int *disconnected);

#endif /* MY_LIB_UILS_H */