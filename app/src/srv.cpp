#include <unistd.h>
#include <fcntl.h>
#include <cstring>
#include <arpa/inet.h>
#include <cmath>
#include <iostream>
#include <thread>

#include "lib.h"

static void data_generate_and_send_task(int sockfd);

int main(int argc, char const *argv[])
{
	char addr[20] = {0};
	int sockfd = 0;
	const char *pr_id = PROVIDER_ID;

	if (libaio_server_init(PORTN) < 0) {
		perror("SRV: Unable to initialize server");
		exit(EXIT_FAILURE);
	}

	sprintf(addr, IPADDRESS);

	if((sockfd = libaio_register_client(addr, PORTN, NULL)) < 0) {
		perror("SRV: Unable to register server");
		exit(EXIT_FAILURE);
	} else {
		std::cout << "SRV: Server registered." << std::endl;
	}

	if (fcntl(sockfd, F_SETFL, O_NONBLOCK)) {
		perror("SRV: Failed to set socket to non-blocking mode");
	}

	/* Send greetings to the server */
	send(sockfd, pr_id, MSGLEN, 0);

	std::thread data_generate(data_generate_and_send_task, sockfd);

	data_generate.join();

	return 0;
}

static void data_generate_and_send_task(int sockfd)
{
	double radians = -M_PI/2;
	double delta = 0.0;
	int speed = 0;
	char buffer[MSGLEN] = {0};
	int increment = 0;

	srand(time(NULL));

	increment = rand()%104 + 26;
	delta = (double)increment/10000;

	std::cout << "SRV: Data generation task successfully started!" << std::endl;

	while(1) {
		speed = (int)((sin(radians) + 1.0) * MAXSPEED);
		snprintf(buffer, MSGLEN - 1, "%d", speed);
		send(sockfd, buffer, MSGLEN, 0);
		radians += delta;
		if (radians > (1.5*M_PI)) {
			increment = rand()%104 + 26;
			delta = (double)increment/10000;
			radians = -M_PI/2;
		}
		usleep(8000);
	}
}
