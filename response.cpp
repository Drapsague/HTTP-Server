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

void Response::recv_request() {
	// Receiving incomming requests
	ssize_t rec {};
	/*int count {};*/
	do {
		ssize_t rec = recv(connection_->m_clientSocket, m_recvBuffer.get(), m_resBuffer_size, 0);
		if (rec <= 0) {
			std::cerr << "Client disconnected" << '\n';
			close(connection_->m_clientSocket);
			connection_->m_clientSocket = 0;
			return;
		}
		std::cout << '\n';
		/*std::cout << "Mesage received" << '\n';*/
		std::cout << m_recvBuffer.get() << '\n';

	}while(rec != 0);
}

void Response::parse_request() {
	// Here we retreive the string after the end of the first line
	// We get the size of that string and substract it to the size of the whole request
	char* body_request {strstr(m_recvBuffer.get(), "\r\n")};
	size_t fline_size  {strlen(m_recvBuffer.get()) - strlen(body_request)};

	// Creating a string with the exact size of the first line
	// And copying the first line in that string
	// Assigning the header to our class argument
	m_header = std::make_unique<char[]>(fline_size + 1);
	strncpy(m_header.get(), m_recvBuffer.get(), fline_size);
	m_header.get()[fline_size] = '\0';

}

bool Response::end_requests() {
	char* body_request {strstr(m_recvBuffer.get(), "\r\n\r\n")};
	/*size_t fline_size  {strlen(m_recvBuffer.get()) - strlen(body_request)};*/
	if (body_request) {
		std::cout << "End requests" << '\n';
		return false;
	}
	return true;
}


void Response::create_response() {
	/*std::cout << "create response" << std::endl;*/
	if (connection_->m_clientSocket == 0) { return;}

	// Sending a response to the client
	// Creating the reponse buffer
	/*std::random_device rd;*/
	/*std::mt19937 gen(rd());*/
	/*std::uniform_int_distribution<int> dist(1, 100);*/
	/**/
	/*int random_number = dist(gen);*/
	/*char payload_buffer[100];*/
	/*snprintf(payload_buffer, sizeof(payload_buffer), "Welcome to the Grid %d", random_number);*/
	
	const char* payload {m_header.get()};
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
	
	if (response >= (static_cast<int>(m_resBuffer_size) - m_response)) {
		std::cerr << "Response was truncated" << '\n';
	}
	else if (response < 0) {
		std::cerr << "Error formating response" << std::endl;
		return;
	}
	m_response = response;
}


void Response::send_response() {
	if (connection_->m_clientSocket == 0) { return;}
	/*std::cout << "send response" << std::endl;*/
	// Here we need to send in a loop, send() could send the response partially
	// We make sure that every bytes is sent
	size_t rest_bytes = m_response;
	// Be careful to not delete or nullptr current, .get() only gives a temporary access
	// Current can be used if buffer_se is not reset or nullptr
	char* current = m_resBuffer.get();
	std::cout << '\n';
	/*std::cout << current << '\n';*/
	while (rest_bytes != 0) {
		size_t X = std::min(rest_bytes, m_resBuffer_size);
		ssize_t sendResponse = send(connection_->m_clientSocket, current, X, 0);
		rest_bytes -= sendResponse;
		current += sendResponse;
		if (sendResponse == -1) {
			current = nullptr;
			std::cerr << "Failed to send a response to the client." << '\n';
			return;
		}
	}
}
