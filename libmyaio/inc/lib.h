#ifndef MY_LIB_H
#define MY_LIB_H

#define IPADDRESS "127.0.0.1"
#define PROVIDER_ID "This is provider"
#define MSGLEN (50)
#define PORTN (12345)
#define MAX_ATTEMPTS (100)
#define MAXCONN (5)
#define MAXSPEED (140) /* 280/2 */

struct client_str {
	int sockfd;
	int (*callback)(void);
};

#ifdef __cplusplus
extern "C"{
#endif

int libaio_server_init(int portnum);
int libaio_register_client(char addr[20], int portnum, struct client_str *client);

#ifdef __cplusplus
}
#endif

#endif /* MY_LIB_H */
