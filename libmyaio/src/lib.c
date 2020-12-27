#include "utils.h"
#include "lib.h"

static void *wait_for_new_connection_task(void *args);
static void *data_receiver_task(void *args);

pthread_t wait_for_new_connection;
pthread_t data_receiver;

static int libaio_get_data(void);


/* This procedure configures and starts server instance			*
 * The server manages connections and works as message broker	*
 * between data provider and clients							*/
int libaio_server_init(int portnum) {
	int srv_socket = 0;
	struct sockaddr_in srv_addr;

	printf("libAIO: Starting AIO server at port %d...\n", portnum);

	if ((srv_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("libAIO: socket creation failed");
		return -1;
	}

	srv_addr.sin_family = AF_INET;
	srv_addr.sin_port = htons(portnum);
	srv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	bzero(&srv_addr.sin_zero, sizeof(srv_addr.sin_zero)); 

	if (bind(srv_socket, (struct sockaddr*) &srv_addr, sizeof(srv_addr)))
	{
		perror("libAIO: bind() failed");
		return -1;
	}

	if (listen(srv_socket, SOMAXCONN))
	{
		perror("libAIO: listen() failed");
		return -1;
	} else {
		printf("libAIO: Server successfully established.\n");
	}

	if (pthread_create(&wait_for_new_connection, NULL, wait_for_new_connection_task, (void *)&srv_socket) != 0)
	{
		perror("libAIO: connection waiting task failed to start\n");
		return -1;
	}

	printf("libAIO: Server is waiting for connections!\n");
	sleep(1);

	return 0;
}


/* This procedure connects data provider and clients to the server	*
 * and starts data_receiver thread instance for each client			*/
int libaio_register_client(char addr[20], int portnum, struct client_str *client) {

	int connect_attempts = 0;
	int sockfd;
	struct sockaddr_in servaddr = {0};

	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("libAIO: socket creation failed");
		return -1;
	}

	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(portnum);
	servaddr.sin_addr.s_addr = inet_addr(addr);

	printf("libAIO: CLN: Connecting to the server...\n");

	while (1) {
		if (connect(sockfd, (struct sockaddr*) &servaddr, sizeof(servaddr)) != 0) {
			connect_attempts++;
			sleep(1);
		} else {
			printf("libAIO: CLN: Connected!\n");
			break;
		}
		if (connect_attempts > MAX_ATTEMPTS) {
			printf("\nlibAIO: CLN: connection with the server failed after %d attempts...\n", connect_attempts);
			return -1;
		}
	}

	if (NULL == client) {
		return sockfd;
	} else {
		client->sockfd = sockfd;
		client->callback = libaio_get_data;
	}

	if (pthread_create(&data_receiver, NULL, data_receiver_task, (void *)client) != 0)
	{
		perror("libAIO: Data receiver task failed to start\n");
		return -1;
	}
	printf("libAIO: Data receiver task started\n");

	sleep(1);

	return 0;
}

static void *wait_for_new_connection_task(void *args)
{
	struct pollfd fds[MAXCONN] = {0};
	int srv_socket = *(int *)args;
	struct sockaddr_in cln_addr;
	int poll_result = -1;
	int res = -1;
	int clientnum = 0;
	int disconnected_id = -1;

	fds[clientnum].fd = srv_socket;
	fds[clientnum].events = POLLIN;
	fds[clientnum].revents = 0;
	clientnum++;

	while(1) {
		poll_result = poll(fds, clientnum, -1);
		if (poll_result < 0) {
			perror("libAIO: poll() failed");
			return 0;
		}
		/* poll() was triggered, need to find who was that and what he wants */
		for (int i = 0; i < clientnum; i++) {

			if (fds[i].revents&POLLIN) {

				if(fds[i].fd == srv_socket) {
					/* New connection event */
					printf("libAIO: New connection detected!\n");
					res = utils_accept_new_client(srv_socket, fds, &cln_addr, clientnum);
					if (res > 0) {
						clientnum = res;
					} else {
						printf("libAIO: Unable to accept new client\n");
					}
				} else {
					/* There is a message from someone, check contents and decide what to do next */
					if (utils_interact_with_client(i, fds, &clientnum, &disconnected_id) < 0)
						return 0; /* Data provider is down, exit */
				}
			}
		}
		/* Remove disconnected client from pollfds struct */
		if (disconnected_id > 0) {
			/* !!! Warning! Pay attention on array bounds!!! */
			close(fds[disconnected_id].fd);
			for (int i = disconnected_id; i < (clientnum - 1); i++)
			{
				fds[i].fd = fds[i + 1].fd;
			}
			disconnected_id = -1;
			clientnum--;
		}
	}

	return 0;
}

static int glob_data = 0;

static void *data_receiver_task(void *args) {
	struct client_str *client = (struct client_str *)args;
	int speed_data = 0;
	char buffer[MSGLEN];
	int n = 0;

	while(1) {
		n = recv(client->sockfd, &buffer, MSGLEN, 0);
		if (n == 0) {
			printf("Server has closed connection");
			glob_data = -100;
			return 0;
		}
		buffer[n] = '\0';
		if (sscanf(buffer, "%d", &speed_data) == 1) {
			glob_data = speed_data;
		}
	}

	return 0;
}

static int libaio_get_data(void) {
	return glob_data;
}
