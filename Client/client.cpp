#include "client.h"

#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <unistd.h>
#include <cstring>
#include <thread>


void Client::connection() {
	// Need to create a socket
	int socketfd = socket(AF_INET, SOCK_STREAM, 0);
	m_sockfd = socketfd;

	if (socketfd <= 0){
		std::cerr << "Socket creation failed" << '\n';
		close(m_sockfd);
		return;
	}

	struct sockaddr_in serverAdd;
	// AF_INET is the param for IPV4
	serverAdd.sin_family = AF_INET;
	// htons converts <Port> un bytes
	serverAdd.sin_port = htons(m_port);
	// inet_pton converts <IP> in bytes and handles errors
	inet_pton(AF_INET, "127.0.0.1", &(serverAdd.sin_addr));


	int con = connect(m_sockfd, (struct sockaddr*)&serverAdd, sizeof(serverAdd));

	if (con == -1) {
		std::cerr << "Can't connect to the IP and Port that were given." << std::endl;
		close(m_sockfd);
		return;
	}
}

void Client::response() {
	// By nesting those function in a while loop we can send requests no stop
	// As this function runs in a thread of its own the while is required
	while (m_sockfd > 0) {
		create_response();
		send_response();
	}
}

void Client::receive() {
	// By nesting those function in a while loop we can receive requests no stop
	// As this function runs in a thread of its own the while is required
	while (m_sockfd > 0) {
		recv_request();
	}
}

void Client::create_response() {
	if (m_sockfd < 0) { return;}
	std::memset(m_payload.get(), 0, m_resBuffer_size);

	// Sending a response to the client
	// Creating the reponse buffer
	
	m_response = 0;
	std::cout << "Enter message : ";
	std::cin >> m_payload.get();
	
	char* stop_paylaod {strstr(m_payload.get(), "QUIT")};
	if (stop_paylaod) {
		close(m_sockfd);
		m_sockfd = -1;
	}
	// Adding 2 because we add \r\n after the payload
	size_t payload_size {strlen(m_payload.get()) + 2};

	// Content-Length needs to match the size of the payload
	m_resBuffer = std::make_unique<char[]>(m_resBuffer_size);
	int response = snprintf(m_resBuffer.get(), m_resBuffer_size, 
			 "GET /database.json HTTP/1.1\r\n"
			 "Content-Type: application/json\r\n"
			 "Content-Length: %d\r\n"
			 "Connection : keep-alive\r\n"
			 "\r\n"
			 "{%s\r\n",
			 static_cast<int>(payload_size),
			 m_payload.get());
	
	if (response >= static_cast<int>(m_resBuffer_size)) {
		std::cerr << "Response was truncated" << '\n';
	}
	else if (response < 0) {
		std::cerr << "Error formating response" << std::endl;
		return;
	}
	m_response = response;
	
}


void Client::send_response() {
	if (m_sockfd < 0) { return;}
	// Here we need to send in a loop, send() could send the response partially
	// We make sure that every bytes is sent
	size_t rest_bytes = m_response;
	// Be careful to not delete or nullptr current, .get() only gives a temporary access
	// Current can be used if buffer_se is not reset or nullptr
	char* current = m_resBuffer.get();
	std::cout << '\n';
	std::cout << "Response requests --- \n" << current << '\n';
	// while (rest_bytes != 0) {
	//
	// }
	size_t X = std::min(rest_bytes, m_resBuffer_size);
	ssize_t sendResponse = send(m_sockfd, current, X, 0);
	rest_bytes -= sendResponse;
	current += sendResponse;
	if (sendResponse == -1) {
		current = nullptr;
		std::cerr << "Failed to send a response to the client." << '\n';
		return;
	}
}


ssize_t Client::recv_request() {
	// Receiving incomming requests
	std::memset(m_recvBuffer.get(), 0, m_resBuffer_size);
	ssize_t rec {};
	// do {
	// }while(rec != 0);
	rec = recv(m_sockfd, m_recvBuffer.get(), m_resBuffer_size, 0);
	if (rec <= 0) {
		std::cerr << "Client disconnected" << '\n';
		close(m_sockfd);
		m_sockfd = -1;
		return rec;
	}
	std::cout << '\n';
	std::cout << "Mesage received" << '\n';
	std::cout << m_recvBuffer.get() << '\n';
	return rec;


}
