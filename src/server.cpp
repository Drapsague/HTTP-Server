#include "../include/server.h"
#include "../include/requestsHandler.h"

#include <iostream>
#include <memory>
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
	
	// When we set event[x], x is the number of event we can process at the same time
	// Same for change_event[x], x is the number of event we cn register with kqueue at the same time
	struct kevent change_event[4];
	struct kevent event[4];
	
	// Start listenning, 5 is the limit of connections in the queue
	listen(sockfd, 5);
	std::cout << "Listenning on port " << m_port << std::endl;

	// We create a list of events
	kq = kqueue();
	// We create a new event with our server socket
	// So each it is triggered we can process the event
	EV_SET(change_event, sockfd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, 0);

	// Register kevent with the kqueue.
	// kevent(kq, change_event, 1, NULL, 0, NULL) adds the event
	if (kevent(kq, change_event, 1, NULL, 0, NULL) == -1)
	{
		perror("kevent");
		exit(1);
	}
	RequestsHandler req {this};
	int count {};
	for (;;) 
	{
		// new_event returns the number of triggered events and -1 on error
		new_events = kevent(kq, NULL, 0, event, 1, NULL);
		if (new_events == -1)
		{
		    perror("kevent");
		    exit(1);
		}
		// This loop each time an event is triggered
		// Each time a socket is triggered, it is added to the event array
		// Then we iterate through this array for each event triggered at the same time
		for (int i = 0; new_events > i; i++) {
			// We can retreive sockets that were triggered
			int event_fd = event[i].ident;

			// When the client disconnects an EOF is sent. By closing the file
			// descriptor the event is automatically removed from the kqueue.
			if (event[i].flags & EV_EOF)
			{
				auto closed_con = m_con_list.find(event_fd);
				if (closed_con != m_con_list.end())
				{
					// Delete instance when the client connection is closed
					std::cerr << "Client has disconnected -- " << event_fd << '\n';
					// Delete the value and key from the map
					m_con_list.erase(closed_con);
				}
				close(event_fd);
			}
			else if (event_fd == sockfd) 
			{
				// This means that there is an incoming connection
				// Like a client socket binded to our server socket
				clientSocket =  accept(sockfd, nullptr, nullptr);


				req.createResponse(clientSocket);

				// To count instances
				count++;
				req.m_res->m_count = count;

				std::shared_ptr<Response> instance_ptr = req.m_res;
				m_con_list.insert({clientSocket, instance_ptr});

				if (clientSocket == -1) 
				{
					std::cerr << "Client socket error -- " << clientSocket << '\n';
				}
				// We add an other event but with the client socket
				EV_SET(change_event, clientSocket, EVFILT_READ, EV_ADD, 0, 0, 0);
				// Then register it with the kqueue
				if (kevent(kq, change_event, 1, NULL, 0, NULL) < 0)
				{
				    perror("kevent error");
				}
			}
			else if (event[i].filter & EVFILT_READ) 
			{
				// I need to figure out how to not create a new instance each requests
				auto it = m_con_list.find(event_fd);
				if (it != m_con_list.end())
				{
					req.handleClient(it->second);
				}
			}

			// Just to see the map, and active connections
			for (const auto &pair : m_con_list) {
				std::cout << pair.first << " " << pair.second << '\n';
    }

			

		}

	}
}


void Server::stop() {
	std::cout << "Closing sockets" << '\n';
	close(sockfd);
	return;
}





