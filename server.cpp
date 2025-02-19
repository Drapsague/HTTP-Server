#include "server.h"

#include <asm-generic/socket.h>
#include <cstddef>
#include <cstdio>
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <unistd.h>
#include <cstring>
#include <memory>

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

	if (bind_settings == -1) {
		std::cerr << "Can't re-use the IP and Port given." << std::endl;
	}

	// Bind the socket to the port and IP we passed in
	int bind_socket = bind(socket_edp, (struct sockaddr*)&serverAdd, sizeof(serverAdd));

	if (bind_socket == -1) {
		std::cerr << "Can't bind the socket to the IP and Port that were given." << std::endl;
		close(socket_edp);
		return;
	}

	// Start listenning, 5 is the limit of connections in the queue
	listen(socket_edp, 5);

	std::cout << "Listenning on port " << m_port << std::endl;

	int clientSocket =  accept(socket_edp, nullptr, nullptr);

	// Creating the buffer for incomming requests
	size_t buffer_size {4096};
	std::unique_ptr<char[]> buffer_re {new char[buffer_size]};

	// Receiving incomming requests
	ssize_t rec = recv(clientSocket, buffer_re.get(), buffer_size, 0);
	if (rec <= 0) {
		std::cerr << "Client disconnected or an error occured" << std::endl;
		close(socket_edp);
		close(clientSocket);
		return;
	}
	std::cout << buffer_re.get() << std::endl;

	// Sending a response to the client
	// Creating the reponse buffer
	size_t buffer_size_se {4096};
	std::unique_ptr<char[]> buffer_se {new char[buffer_size_se]};

	// Creating payload
	const char* payload {"Welcome to the Grid"};
	// Adding 2 because we add \r\n after the payload
	size_t payload_size {strlen(payload) + 2};
	/*const int payload_size = 4096;*/
	/*char payload[payload_size] = "Hello World!";*/

	// Content-Length needs to match the size of the payload
	int response = snprintf(buffer_se.get(), buffer_size_se, 
			 "HTTP/1.1 200 OK\r\n"
			 "Content-Type: text/html\r\n"
			 "Content-Length: %d\r\n"
			 "\r\n"
			 "%s\r\n",
			 static_cast<int>(payload_size),
			 payload);
	
	if (response >= static_cast<int>(buffer_size_se)) {
		std::cerr << "Response was truncated" << std::endl;
	}
	else if ( response < 0 ) {
		std::cerr << "Error formating response" << std::endl;
		close(socket_edp);
		close(clientSocket);
		return;
	}

	// Here we need to send in a loop, send() could send the response partially
	// We make sure that every bytes is sent
	size_t rest_bytes = response;
	std::cout << "Response size is : " << response << std::endl;
	char* current = buffer_se.get();
	while (rest_bytes != 0) {
		size_t X = std::min(rest_bytes, buffer_size_se);
		ssize_t sendResponse = send(clientSocket, current, X, 0);
		rest_bytes -= sendResponse;
		current += sendResponse;
		if (sendResponse == -1) {
			std::cerr << "Failed to send a response to the client." << std::endl;
			close(socket_edp);
			close(clientSocket);
			return;
		}
	}
	
	std::cout << "Closing connection" << std::endl;
	current = nullptr;
	close(socket_edp);
	close(clientSocket);
		
}


