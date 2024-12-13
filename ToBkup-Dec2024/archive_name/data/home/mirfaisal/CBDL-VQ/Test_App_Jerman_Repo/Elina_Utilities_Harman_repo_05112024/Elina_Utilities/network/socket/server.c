// Server Program
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include <poll.h>
#include "common.h"

#ifndef POLLRDHUP
#define POLLRDHUP			0x20a
#endif

#define SEND_MSG			1
#define RECV_MSG			0

static unsigned int conn_accept_cnt = 0;
static struct socket_info	server, client[MAX_CONN];
static struct pollfd 		pollfds[MAX_CONN];

static void *server_connection_handler(void *arg)
{
	int 		index = 0, cnt = 0, poll_ret = 0;

	while (1)
	{
		if ((poll_ret = poll(pollfds, conn_accept_cnt, 10)) <= 0)
		{
			printf("ERROR: Poll failed with return value %d\n", poll_ret);
		}

		for (index = 0; index < conn_accept_cnt; index++, cnt++)
		{
#if SEND_MSG
			if (pollfds[index].revents & POLLOUT)
			{
				sprintf(client[index].server_data, "Client ID is %d\n", client[index].fd);
				send(client[index].fd, client[index].server_data, strlen(client[index].server_data), 0);
				printf("MSG sent to Client [%d] : %s\n", client[index].fd, client[index].server_data);
			}
#endif
#if RECV_MSG
			if (pollfds[index].revents & POLLIN)
			{
				read(client[index].fd, client[index].client_data, sizeof(client[index].client_data));
				printf("Total %d Clients ==> client [%d] MSG: %s\n", conn_accept_cnt, client[index].fd, client[index].client_data);
				memset(client[index].client_data, 0, sizeof(client[index].client_data));
			}
#endif
			switch (pollfds[index].revents)
			{
				case POLLPRI :
				case POLLRDHUP :
				case POLLERR :
				case POLLHUP :
				case POLLNVAL :
				case POLLRDNORM : 
				case POLLRDBAND :
				case POLLWRNORM :
				case POLLWRBAND :
					printf("Error: Other poll trigger %d\n", pollfds[index].revents);
					break;
			}


			fflush(stdin);
			fflush(stdout);
		}

		sleep(1);
	}

	return NULL;
}


int main(int argc, char const *argv[])
{
	unsigned int 		backlog = 0, acceptance_delay = 0, client_addr_len = 0;
	int 			opt = 1;
	pthread_t		thread_id = 0;

	if (argc != 3)
	{
		printf("Error: Usage ==> %s <backlog value> <accept delay>\n", argv[0]);
		exit(0);
	}

	backlog = atoi(argv[1]);
	acceptance_delay = atoi(argv[2]);

	// Creating socket file descriptor
	if ((server.fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		perror("socket failed\n");
		exit(EXIT_FAILURE);
	}

	// Forcefully attaching socket to the port 
	if (setsockopt(server.fd, SOL_SOCKET, (SO_REUSEADDR | SO_REUSEPORT), &opt, sizeof(opt)) != 0)
	{
		perror("setsockopt failed\n");
		exit(EXIT_FAILURE);
	}

	server.address_info.sin_family = AF_INET;
	server.address_info.sin_addr.s_addr = inet_addr(SERVER_IP);
	server.address_info.sin_port = htons(SERVER_PORT);

	// Forcefully attaching socket to the port 8080
	if (bind(server.fd, (struct sockaddr *)&server.address_info, sizeof(server.address_info)) != 0)
	{
		perror("bind failed\n");
		exit(EXIT_FAILURE);
	}

	/* Listen for client - passive mode or blocked */
	if (listen(server.fd, backlog) != 0)
	{
		perror("listen");
		exit(EXIT_FAILURE);
	}

	if (pthread_create(&thread_id, NULL, &server_connection_handler, NULL) != 0)
	{
		perror("Server's Thread Creation failed\n");
		close(server.fd);
		exit(EXIT_FAILURE);
	}

	while (1)
	{
		sleep(acceptance_delay);
		//usleep(acceptance_delay);

		client_addr_len = sizeof(client[conn_accept_cnt].address_info);

		if ((client[conn_accept_cnt].fd = accept(server.fd, (struct sockaddr *)&client[conn_accept_cnt].address_info, (socklen_t*)&client_addr_len)) < 0)
		{
			perror("accept");
			exit(EXIT_FAILURE);
		}

		pollfds[conn_accept_cnt].fd = client[conn_accept_cnt].fd;
		pollfds[conn_accept_cnt].events = POLLIN | POLLOUT | POLLPRI | POLLRDHUP | POLLERR | POLLHUP | POLLNVAL | POLLRDNORM | POLLRDBAND | POLLWRNORM | POLLWRBAND;

		printf("New Connection! Total Clients Connected %d\n", ++conn_accept_cnt);
	}

	return 0;
}
