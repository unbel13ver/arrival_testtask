#include "utils.h"
#include "lib.h"

static int utils_check_if_data_provider(char *buffer, int length) {
	char teststring[MSGLEN] = {0};
	int res;

	strncpy(teststring, buffer, length);

	res = strncmp(teststring, PROVIDER_ID, length);

	if (res == 0)
		return 1;

	return 0;
}

int utils_accept_new_client(int srv_socket, struct pollfd *fds, struct sockaddr_in *cln_addr, int clientnum) {
	int cln_socket = 0;
	socklen_t len = sizeof(*cln_addr);

	cln_socket = accept(srv_socket, (struct sockaddr*) cln_addr, &len);
	if (clientnum < MAXCONN) {
		if (cln_socket < 0) {
			perror("libAIO: accept() failed");
			return -1;
		}
		if (fcntl(cln_socket, F_SETFL, O_NONBLOCK)) {
			perror("failed to set non-blocking mode");
		}
		fds[clientnum].fd = cln_socket;
		fds[clientnum].events = POLLIN;
		fds[clientnum].revents = 0;
		clientnum++;
	} else {
		shutdown(cln_socket, 2);
		close(cln_socket);
		printf("libAIO: maximum connections achieved!\n");
	}

	return clientnum;
}

/* Inline candidate */
int utils_interact_with_client(int client_id, struct pollfd *fds, int *clientnum, int *disconnected) {
	int nbytes = 0;
	char buffer[MSGLEN];
	int cnum = *clientnum;
	static int provider = 0;

	nbytes = recv(fds[client_id].fd, buffer, MSGLEN, 0);

	if ((!provider) && (nbytes != 0)) {
		if (utils_check_if_data_provider(buffer, nbytes)) {
			provider = client_id;
		}
	}

	if (nbytes == 0) {
		/* Provider is down, close all sockets and exit */
		if (client_id == provider) {
			for (int j = 1; j < cnum; j++) {
				close(fds[j].fd);
				return -1;
			}
		} else {
			*disconnected = client_id;
		}
	} else if (nbytes > 0) {
		/* Send payload to clients */
		for (int j = 1; j < cnum; j++) {
			if (j != client_id) {
				if (send(fds[j].fd, buffer, MSGLEN, 0) < 0) {
					/* Looks like someone has disconnected */
				}
			}
		}
	}

	return 0;
}

