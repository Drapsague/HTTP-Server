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


void Response::create_response() {
	/*std::cout << "create response" << std::endl;*/

	// Sending a response to the client
	// Creating the reponse buffer

	const char* payload {"Welcome to the Grid"};
	// Adding 2 because we add \r\n after the payload
	size_t payload_size {strlen(payload) + 2};
	/*const int payload_size = 4096;*/
	/*char payload[payload_size] = "Hello World!";*/

	// Content-Length needs to match the size of the payload
	int response = snprintf(m_resBuffer.get(), m_resBuffer_size, 
			 "HTTP/1.1 200 OK\r\n"
			 "Content-Type: text/html\r\n"
			 "Content-Length: %d\r\n"
			 "\r\n"
			 "%s\r\n",
			 static_cast<int>(payload_size),
			 payload);
	
	if (response >= static_cast<int>(m_resBuffer_size)) {
		std::cerr << "Response was truncated" << '\n';
	}
	else if ( response < 0 ) {
		std::cerr << "Error formating response" << std::endl;
		close(m_sockfd);
		close(m_sockClient);
		return;
	}
	m_response = response;
}


void Response::send_response() {
	/*std::cout << "send response" << std::endl;*/
	// Here we need to send in a loop, send() could send the response partially
	// We make sure that every bytes is sent
	size_t rest_bytes = m_response;
	// Be careful to not delete or nullptr current, .get() only gives a temporary access
	// Current can be used if buffer_se is not reset or nullptr
	char* current = m_resBuffer.get();
	while (rest_bytes != 0) {
		size_t X = std::min(rest_bytes, m_resBuffer_size);
		ssize_t sendResponse = send(m_sockClient, current, X, 0);
		rest_bytes -= sendResponse;
		current += sendResponse;
		if (sendResponse == -1) {
			current = nullptr;
			std::cerr << "Failed to send a response to the client." << '\n';
			close(m_sockfd);
			close(m_sockClient);
			return;
		}
	}
}
