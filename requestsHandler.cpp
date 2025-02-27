#include "requestsHandler.h"
#include "request.h"
#include "response.h"

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


void RequestsHandler::recvRequest() {
	// Creating the buffer for incomming requests
	size_t buffer_size {4096};
	std::unique_ptr<char[]> buffer_re {new char[buffer_size]};

	// Receiving incomming requests
	ssize_t rec = recv(m_clientSocket, buffer_re.get(), buffer_size, 0);
	if (rec <= 0) {
		std::cerr << "Client disconnected or an error occured" << std::endl;
		close(m_sockfd);
		close(m_clientSocket);
		/*throw "Client disconnected or an error occured";*/
		return;
	}
	std::cout << buffer_re.get() << std::endl;

}


int RequestsHandler::parseRequest() {
	// Sending a response to the client
	// Creating the reponse buffer

	const char* payload {"Welcome to the Grid"};
	// Adding 2 because we add \r\n after the payload
	size_t payload_size {strlen(payload) + 2};
	/*const int payload_size = 4096;*/
	/*char payload[payload_size] = "Hello World!";*/

	// Content-Length needs to match the size of the payload
	int response = snprintf(m_sendBuffer.get(), m_sendBuffer_size, 
			 "HTTP/1.1 200 OK\r\n"
			 "Content-Type: text/html\r\n"
			 "Content-Length: %d\r\n"
			 "\r\n"
			 "%s\r\n",
			 static_cast<int>(payload_size),
			 payload);
	
	if (response >= static_cast<int>(m_sendBuffer_size)) {
		std::cerr << "Response was truncated" << '\n';
	}
	else if ( response < 0 ) {
		std::cerr << "Error formating response" << std::endl;
		close(m_sockfd);
		close(m_clientSocket);
		return -1;
	}

	return response;
}

void RequestsHandler::sendResponse() {
	// Here we need to send in a loop, send() could send the response partially
	// We make sure that every bytes is sent
	
	int response = parseRequest();
	size_t rest_bytes = response;
	// Be careful to not delete or nullptr current, .get() only gives a temporary access
	// Current can be used if buffer_se is not reset or nullptr
	char* current = m_sendBuffer.get();
	while (rest_bytes != 0) {
		size_t X = std::min(rest_bytes, m_sendBuffer_size);
		ssize_t sendResponse = send(m_clientSocket, current, X, 0);
		rest_bytes -= sendResponse;
		current += sendResponse;
		if (sendResponse == -1) {
			current = nullptr;
			std::cerr << "Failed to send a response to the client." << '\n';
			close(m_sockfd);
			close(m_clientSocket);
			return;
		}
	}
	current = nullptr;
}



void RequestsHandler::createRequest() {
	/*std::cout << "A" << std::endl;*/
	m_req.recv_request();
	m_req.parse_request(); 
}

void RequestsHandler::createResponse() {
	/*std::cout << "B" << std::endl;*/
	m_res.create_response();
	m_res.send_response();
}

void RequestsHandler::handleClient() {
	createRequest();
	createResponse();
}

