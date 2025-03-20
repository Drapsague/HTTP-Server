#include "../include/response.h"
#include "../include/requestsHandler.h"

#include <cstddef>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <unistd.h>
#include <cstring>
#include <memory>


Response::Response(RequestsHandler* con ,size_t recvBuffer_size)
	: connection_ {con}, m_recvBuffer_size {recvBuffer_size}
{
	std::cout << "creating new RequestHandler instance for the socket : " << connection_->m_clientSocket << '\n';
}

void Response::recv_request() {
	// Receiving incomming requests
	// The recv buffer seems to be very sensitive, we need to flush the buffer to make sure there's no memmory leaks
	// The issue was part of the last requet was in the new one
	/*ssize_t rec {};*/
	std::memset(m_recvBuffer.get(), 0, m_recvBuffer_size);
	std::memset(m_resBuffer.get(), 0, m_resBuffer_size);

	ssize_t rec = recv(connection_->m_clientSocket, m_recvBuffer.get(), m_recvBuffer_size, 0);
	if (rec > 0) {
		m_recvBuffer.get()[rec] = '\0';
	}
	if (rec <= 0) {
		std::cerr << "Client disconnected" << '\n';
		close(connection_->m_clientSocket);
		connection_->m_clientSocket = -1;
		return;
	}
	std::cout << '\n';
	std::cout << "Mesage received -- size : " << rec << '\n';
	std::cout << m_recvBuffer.get() << '\n';
}

std::unique_ptr<char[]> Response::get_header_file() {
	if (connection_->m_clientSocket < 0) { return nullptr;}

	// Here we retreive the string after the end of the first line
	// We get the size of that string and substract it to the size of the whole request
	char* without_get {m_recvBuffer.get() + 4}; // Getting the string without "GET" -> 4, but if "POST" -> 5
	char* f_line {strchr(without_get, ' ')};
	size_t file_s  {strlen(without_get) - strlen(f_line)};

	// Creating a string with the exact size of the first line
	// And copying the first line in that string
	// Assigning the header to our class argument
	std::unique_ptr<char[]> header {new char(file_s + 8)};

	// All the html files are in /public
	memcpy(header.get(), "/public", 7);
	// We copy the /<file>.html in the header
	memcpy(header.get() + 7, without_get, file_s);

	header.get()[file_s + 8] = '\0';

	std::cout << header.get() << '\n';
	return header;
}


std::unique_ptr<char[]> Response::get_file(char* header_ptr) {
	is_valid_header = false;
	std::unique_ptr<char[]> file_ptr {};
	if (!header_ptr) {
		std::cerr << "Header is a nullptr" << '\n';
		is_valid_header = false;
		return file_ptr;
	}
	// We need this to read the file (kind like a socket)
	std::ifstream file(header_ptr + 1, std::ios::binary);
	if (!file) {
		std::cerr << "Error while getting the file" << '\n';
		is_valid_header = false;
		return file_ptr;
	}
	// Getting the size of the file by going to the end and mesure from the start
	file.seekg(0, std::ios::end);
	std::streamsize file_size = file.tellg();
	file.seekg(0, std::ios::beg);

	// Creating the string
	file_ptr = std::make_unique<char[]>(file_size + 1);
	file_ptr.get()[file_size] = '\0';

	// Reading the file, and copying it in the string
	file.read(file_ptr.get(), file_size);
	file.close();

	std::cout << file_ptr.get() << '\n';
	is_valid_header = true;

	return file_ptr;
}


bool Response::end_requests() {
	char* body_request {strstr(m_recvBuffer.get(), "\r\n\r\n")};
	if (body_request) {
		std::cout << "End requests" << '\n';
		return false;
	}
	return true;
}

void Response::create_response() {
	if (connection_->m_clientSocket < 0) { return;}

	// Sending a response to the client
	// Creating the reponse buffer
	std::unique_ptr<char[]> header = get_header_file();

	std::unique_ptr<char[]> file = get_file(header.get());

	if (this->is_valid_header ==  false) {

		int response = snprintf(m_resBuffer.get(), m_resBuffer_size, 
				 "HTTP/1.1 404 Not Found\r\n"
				 "Content-Type: text/html\r\n"
				 "Content-Length: 0\r\n"
				 "\r\n\r\n" );
		m_response = response;
		return;
	}

	
	m_response = 0;
	// Adding 2 because we add \r\n after the payload
	size_t payload_size {strlen(file.get()) + 2};

	// Content-Length needs to match the size of the payload
	int response = snprintf(m_resBuffer.get(), m_recvBuffer_size, 
			 "HTTP/1.1 200 OK\r\n"
			 "Content-Type: text/html\r\n"
			 "Content-Length: %d\r\n"
			 "\r\n"
			 "%s\r\n",
			 static_cast<int>(payload_size),
			 file.get());
	
	if (response >= static_cast<int>(m_resBuffer_size)) {
		std::cerr << "Response was truncated" << '\n';
		return;
	}
	else if (response < 0) {
		std::cerr << "Error formating response" << '\n';
		return;
	}
	m_response = response;
}


void Response::send_response() {
	if (connection_->m_clientSocket <  0) { return;}
	// Here we need to send in a loop, send() could send the response partially
	// We make sure that every bytes is sent
	size_t rest_bytes = m_response;
	// Be careful to not delete or nullptr current, .get() only gives a temporary access
	// Current can be used if buffer_se is not reset or nullptr
	char* current = m_resBuffer.get();

	// We calculate X by taking the min value between rest bytes to send and the buffer size
	size_t X = std::min(rest_bytes, m_resBuffer_size);
	ssize_t sendResponse = send(connection_->m_clientSocket, current, X, 0);
	rest_bytes -= sendResponse;
	current += sendResponse;
	if (sendResponse == -1) {
		current = nullptr;
		std::cerr << "Failed to send a response to the client." << '\n';
		return;
	}
	current = nullptr;
}
