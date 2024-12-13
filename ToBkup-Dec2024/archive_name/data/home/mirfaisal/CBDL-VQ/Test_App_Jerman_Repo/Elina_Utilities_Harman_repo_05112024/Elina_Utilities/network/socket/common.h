#ifndef _COMMON_H_
#define _COMMON_H_

#define MAX_CONN				1024
#define MAX_MSG_PAYLOAD				50

#define SERVER_PORT 				8080
#define SERVER_IP				"127.0.0.1"

struct socket_info
{
	struct sockaddr_in		address_info;
	int 				fd;
	char				server_data[MAX_MSG_PAYLOAD];
	char				client_data[MAX_MSG_PAYLOAD];
};

#endif
