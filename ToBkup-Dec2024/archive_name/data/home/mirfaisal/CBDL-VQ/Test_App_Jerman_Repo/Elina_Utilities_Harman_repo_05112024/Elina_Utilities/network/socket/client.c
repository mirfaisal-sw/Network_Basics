// Client side C/C++ program to demonstrate Socket programming

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <signal.h>
#include <poll.h>
#include <pthread.h>
#include <semaphore.h>
#include "common.h"

#ifndef POLLRDHUP
#define POLLRDHUP			0x20a
#endif

#define CLIENT_PORT 			8080
#define SEND_MSG			0
#define RECV_MSG			1

static int req_clients_count = MAX_CONN, total_clients = 0;
static struct socket_info	server, client[MAX_CONN];
static struct pollfd 		pollfds[MAX_CONN];

static void *clients_connection_handler(void *arg)
{
	int 		trigger_index = 0;
	int 		poll_ret = 0;
	int 		read_len = 0;

	while(1)
	{
		if ((poll_ret = poll(pollfds, req_clients_count, 10)) <= 0)
		{
			//printf("ERROR: Poll failed with return value %d\n", poll_ret);
		}

		for (trigger_index = 0; (trigger_index < req_clients_count); trigger_index++)
		{
#if RECV_MSG
			if (pollfds[trigger_index].revents & POLLIN)
			{
				read_len = read(client[trigger_index].fd, client[trigger_index].client_data, sizeof(client[trigger_index].client_data));
				printf("Total %d Clients ==> Client [%d] MSG: %s\n", total_clients, client[trigger_index].fd, client[trigger_index].client_data);
			}
#endif
#if SEND_MSG
			if (pollfds[trigger_index].revents & POLLOUT)
			{
				sprintf(client[trigger_index].server_data, "Client ID is %d\n", client[trigger_index].fd);
				send(client[trigger_index].fd, client[trigger_index].server_data, strlen(client[trigger_index].server_data), 0);
				memset(client[trigger_index].server_data, 0, sizeof(client[trigger_index].server_data));
			}
#endif

			switch (pollfds[trigger_index].revents)
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
					printf("Error: Other poll trigger %d\n", pollfds[trigger_index].revents);
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
	int 			index = 0;
	pthread_t		thread_id = 0;
	sem_t			sem;

	if (argc != 2)
	{
		printf("Error: Usage ==> %s <number of clients>\n", argv[0]);
		exit(0);
	}

	req_clients_count = atoi(argv[1]);

	if (req_clients_count > MAX_CONN)
	{
		printf("Too many clients, truncating to %d\n", MAX_CONN);
		req_clients_count = MAX_CONN;
	}

	while (1)
	{
		if ((client[index].fd = socket(AF_INET, (SOCK_STREAM | SOCK_NONBLOCK), 0)) == -1)
		{
			perror("socket failed\n");
			exit(EXIT_FAILURE);
		}

		server.address_info.sin_family = AF_INET;
		server.address_info.sin_port = htons(SERVER_PORT);
		server.address_info.sin_addr.s_addr = inet_addr(SERVER_IP);

#if BINDING_CLIENT
		memset(&client[index].address_info, '0', sizeof(client[index].address_info));

		client[index].address_info.sin_family = AF_INET;
		client[index].address_info.sin_port = htons(CLIENT_PORT);
		client[index].address_info.sin_addr.s_addr = inet_addr("127.0.0.1");

		// Forcefully attaching socket to the port 
		if (bind(client[index].fd, (struct sockaddr *)&client[index].address_info, sizeof(client[index].address_info)) != 0)
		{
			perror("bind failed");
			exit(EXIT_FAILURE);
		}
#endif

		if (connect(client[index].fd, (struct sockaddr *)&server.address_info, sizeof(server.address_info)) == -1)
		{
			if (EINPROGRESS != errno)
			{
				printf("Connect failed at %d clients being connected\n", index);
				exit(EXIT_FAILURE);
			}
		}

		// Record fds for poll
		pollfds[index].fd = client[index].fd;
		pollfds[index].events = POLLIN | POLLOUT | POLLPRI | POLLRDHUP | POLLERR | POLLHUP | POLLNVAL | POLLRDNORM | POLLRDBAND | POLLWRNORM | POLLWRBAND;

		index++;

		if (index >= req_clients_count)
		{
			if (pthread_create(&thread_id, NULL, clients_connection_handler, NULL) != 0)
			{
				perror("Client Thread Creation Failed\n");
				exit(EXIT_FAILURE);
			}

			if (!sem_init(&sem, 0, 0))
			{
				total_clients = index;
				sem_wait(&sem);
			}
		}
	}

	return 0;
}
