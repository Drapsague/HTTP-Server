#include "server.h"

#include <cstddef>
#include <cstdio>
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
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

	if (socket_edp == -1) {
		std::cerr << "Socket creation failed." << std::endl;
		exit(1);
	}
	
	struct sockaddr_in serverAdd;
	// AF_INET is the param for IPV4
	serverAdd.sin_family = AF_INET;
	// htons converts <Port> un bytes
	serverAdd.sin_port = htons(m_port);
	// inet_pton converts <IP> in bytes and handles errors
	inet_pton(AF_INET, "127.0.0.1", &(serverAdd.sin_addr));


	// Bind the socket to the port and IP we passed in
	int bind_socket = bind(socket_edp, (struct sockaddr*)&serverAdd, sizeof(serverAdd));

	if (bind_socket == -1) {
		std::cerr << "Can't bind the socket to the IP and port that were given." << std::endl;
		exit(1);
	}


	// Start listenning, 5 is the limit of connections in the queue
	listen(socket_edp, 5);

	std::cout << "Listenning on port " << m_port << std::endl;

	int clientSocket =  accept(socket_edp, nullptr, nullptr);

	// Creating the buffer for incomming requests
	size_t buffer_size {4096};
	char *buffer_re {new char[buffer_size]};

	// Receiving incomming requests
	recv(clientSocket, buffer_re, buffer_size, 0);

	std::cout << buffer_re << std::endl;

	// Sending a response to the client
	// Creating the reponse buffer
	size_t buffer_size_re {4096};
	char *buffer_se {new char[buffer_size_re]};

	// Creating payload
	/*const char *payload {"Welcome to the Grid"};*/
	/*size_t payload_size {strlen(payload)};*/
	const int payload_size = 4096;
	char payload[payload_size] = "Hello World!";

	int response = snprintf(buffer_se, buffer_size_re, 
			 "HTTP/1.1 200 OK\r\n"
			 "Content-Type: text/html\r\n"
			 "Content-Length: %d\r\n"
			 "\r\n"
			 "%s",
			 static_cast<int>(payload_size),
			 payload);
	
	if (response >= static_cast<int>(buffer_size_re)) {
		std::cerr << "Warning: Response was truncated!" << std::endl;
	}
	if (response == -1) {
		std::cerr << "Error formating response" << std::endl;
		delete[] buffer_se;
		delete[] buffer_re;
		exit(1);
	}
	ssize_t sendResponse = send(clientSocket, buffer_se, response, 0);
	
	if (sendResponse == -1) {
		std::cerr << "Failed to send a response to the client." << std::endl;
		delete[] buffer_se;
		delete[] buffer_re;
		exit(1);
	}

	std::cout << "Closing connection" << std::endl;
	delete[] buffer_se;
	delete[] buffer_re;
	buffer_se = nullptr;
	buffer_re = nullptr;
	close(socket_edp);
		


}


