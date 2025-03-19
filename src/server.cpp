#include "../include/server.h"
#include "../include/requestsHandler.h"

#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/event.h>
#include <unistd.h>
#include <cstring>

/* Create a socket
 * Bind the socket to an IP and Port
 * listen for connection
 * accept connection
 * exchange data
 * */

void Server::start() {
	// Initializing our server instance

	// Creates a socket and return the socket file descriptor
	int socket_edp = socket(AF_INET, SOCK_STREAM, 0); 
	sockfd = socket_edp;

	if (socket_edp == -1) 
	{
		std::cerr << "Socket creation failed." << std::endl;
		close(socket_edp);
		return;
	}
	
	struct sockaddr_in serverAdd;
	// AF_INET is the param for IPV4
	serverAdd.sin_family = AF_INET;
	// htons converts <Port> un bytes
	serverAdd.sin_port = htons(m_port);
	// inet_pton converts <IP> in bytes and handles errors
	inet_pton(AF_INET, "127.0.0.1", &(serverAdd.sin_addr));

	// When we close the server the port can be locked temporarily and cause bind() to fail
	// We specify to re-use the same Ip and Port
	int opt = 1;
	int bind_settings = setsockopt(socket_edp, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

	if (bind_settings == -1) 
	{
		std::cerr << "Can't re-use the IP and Port given." << std::endl;
	}

	// Bind the socket to the port and IP we passed in
	int bind_socket = bind(socket_edp, (struct sockaddr*)&serverAdd, sizeof(serverAdd));

	if (bind_socket == -1) 
	{
		std::cerr << "Can't bind the socket to the IP and Port that were given." << std::endl;
		close(socket_edp);
		return;
	}

}


void Server::connection() {
	int kq;
	int new_events;
	int clientSocket;

	struct kevent change_event[4],
		event[4];
	
	// Start listenning, 5 is the limit of connections in the queue
	listen(sockfd, 5);
	std::cout << "Listenning on port " << m_port << std::endl;

	kq = kqueue();
	EV_SET(change_event, sockfd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, 0);

	// Register kevent with the kqueue.
	if (kevent(kq, change_event, 1, NULL, 0, NULL) == -1)
	{
		perror("kevent");
		exit(1);
	}
	for (;;) 
	{
		new_events = kevent(kq, NULL, 0, event, 1, NULL);
		if (new_events == -1)
		{
		    perror("kevent");
		    exit(1);
		}
		std::cout << "A" << '\n';
		for (int i = 0; new_events > i; i++) {

			int event_fd = event[i].ident;
			std::cout << "D" << '\n';

			// When the client disconnects an EOF is sent. By closing the file
			// descriptor the event is automatically removed from the kqueue.
			if (event[i].flags & EV_EOF)
			{
				printf("Client has disconnected");
				close(event_fd);
			}
			else if (event_fd == sockfd) 
			{
				std::cout << "C" << '\n';

				clientSocket =  accept(sockfd, nullptr, nullptr);

				if (clientSocket == -1) 
				{
					std::cerr << "Client socket() error" << '\n';
				}
				EV_SET(change_event, clientSocket, EVFILT_READ, EV_ADD, 0, 0, NULL);
				if (kevent(kq, change_event, 1, NULL, 0, NULL) < 0)
				{
				    perror("kevent error");
				}
			}
			else if (event[i].filter & EVFILT_READ) 
			{
				// I need to figure out how to not create a new instance each requests
				std::cout << "B" << '\n';
				RequestsHandler req {this, event_fd};
				req.handleClient();
			}


		}


	}
}


void Server::stop() {
	std::cout << "Closing sockets" << '\n';
	close(sockfd);
	return;
}

bool Server::is_con_active(int& clientfd) {
	auto cl {m_con_list.find(clientfd)};
	
	if (cl != m_con_list.end()) {
		std::cout << cl->first << ":" << cl->second << " -- Connection still active" << '\n';
		return true;
	}
	std::cerr << "Client not found" << '\n';
	return false;
}

RequestsHandler* Server::get_active_con(int& clientfd) {
	auto cl {m_con_list.find(clientfd)};

	if (cl != m_con_list.end()) {
		return cl->second;
	}
	std::cerr << "Client not found" << '\n';
	return nullptr;
}






