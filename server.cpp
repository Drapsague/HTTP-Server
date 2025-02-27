#include "server.h"
#include "requestsHandler.h"

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
	sockfd = socket_edp;

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

	sockClient = clientSocket;

	RequestsHandler req = RequestsHandler(this, 4096);
	req.handleClient();
	/*req.recvRequest();*/
	/*req.parseRequest();*/
	/*req.sendResponse();*/

	/*std::cout << "Closing connection" << std::endl;*/
	/*close(socket_edp);*/
	/*close(clientSocket);*/

	
}



