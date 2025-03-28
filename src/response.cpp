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


Response::Response(int& clientfd)
	: m_clientSocket {clientfd}
{
	std::cout << "creating new Response instance " << clientfd << '\n';
}

void Response::recv_request() {
	// Receiving incomming requests
	// The recv buffer seems to be very sensitive, we need to flush the buffer to make sure there's no memmory leaks
	// The issue was part of the last requet was in the new one
	std::memset(m_recvBuffer.get(), 0, m_recvBuffer_size);
	std::memset(m_resBuffer.get(), 0, m_resBuffer_size);

	ssize_t rec = recv(m_clientSocket, m_recvBuffer.get(), m_recvBuffer_size, 0);

	std::cout << "---- instance " << m_count << " ----" << '\n';
	if (rec > 0) {
		m_recvBuffer.get()[rec] = '\0';
	}
	if (rec <= 0) {
		std::cerr << "Client disconnected" << '\n';
		close(m_clientSocket);
		m_clientSocket = -1;
		return;
	}
	std::cout << '\n';
	std::cout << "Mesage received -- size : " << rec << '\n';
	std::cout << m_recvBuffer.get() << '\n';
	std::cout << "-----------------------------" << '\n';
}

std::unique_ptr<char[]> Response::get_header_file() {
	if (m_clientSocket < 0) { return nullptr;}

	// Here we retreive the string after the end of the first line
	// We get the size of that string and substract it to the size of the whole request
	char* without_get {m_recvBuffer.get() + 4}; // Getting the string without "GET" -> 4, but if "POST" -> 5
	char* f_line {strchr(without_get, ' ')};
	size_t file_s  {strlen(without_get) - strlen(f_line)};

	// Creating a string with the exact size of the first line
	// And copying the first line in that string
	// Assigning the header to our class argument
	std::unique_ptr<char[]> header {new char(file_s + 7)};

	// All the html files are in /public
	memcpy(header.get(), "public", 6);
	// We copy the /<file>.html in the header
	memcpy(header.get() + 6, without_get, file_s);

	header.get()[file_s + 6] = '\0';

	// std::cout << header.get() << '\n';
	return header;
}

std::unique_ptr<char[]> Response::get_content() {
	if (m_clientSocket < 0) { return nullptr;}

	// Getting the message sent, all messages have the prefix '#' for easier parsing
	// strchr returns a pointer to the fisrt occurence of the char
	char* buffer {m_recvBuffer.get()};
	char* message {strchr(buffer, '{')};

	// When we get the message we have '<message>\r\n'
	// We have to remove the two last char for json parsing reason
	size_t message_size = strlen(message + 1) - 2;

	// Creating a string with the exact size of the first line
	// And copying the first line in that string
	// Assigning the header to our class argument
	std::unique_ptr<char[]> message_content {new char(message_size + 1)};

	memcpy(message_content.get(), message + 1, message_size);

	message_content.get()[message_size] = '\0';

	return message_content;
}

void Response::write_json(const char* header_ptr) {
	std::unique_ptr<char[]> message = get_content();
	
	if (!header_ptr) 
	{
		std::cerr << "Header is a nullptr" << '\n';
		is_valid_header = false;
		return;
	}
	size_t buffer_size = strlen(message.get()) + 46;
	std::unique_ptr<char[]> json_buffer {new char (buffer_size)};

	// Copy the JSON payload in the buffer
	snprintf(json_buffer.get(), buffer_size, 
			"{\n \"user\": \"%d\",\n \"message\": \"%s\"\n},\n",
			m_count,
			message.get());

	// Here we write the JSON we created in the JSON database
	std::ofstream file(header_ptr, std::ios::app);
	if (file.is_open())
	{
		file << json_buffer.get();
		file.close();
	}
}

std::unique_ptr<char[]> Response::get_file(const char* header_ptr) {
	is_valid_header = false;
	std::unique_ptr<char[]> file_ptr {};
	if (!header_ptr) {
		std::cerr << "Header is a nullptr" << '\n';
		is_valid_header = false;
		return file_ptr;
	}
	// We need this to read the file (kind like a socket)
	std::ifstream file(header_ptr, std::ios::binary);
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

	is_valid_header = true;

	return file_ptr;
}


void Response::create_response() {
	if (m_clientSocket < 0) { return;}

	// Sending a response to the client
	// Creating the reponse buffer
	std::unique_ptr<char[]> header = get_header_file();
	
	// We need to store the header because header points to the recv buffer
	// But we modify the buffer in get_content
	// The header ptr does not points correctly because we modify the buffer
	char temp_header[512] {};
	std::strcpy(temp_header, header.get());

	write_json(temp_header);
	
	std::unique_ptr<char[]> file = get_file(temp_header);

	// m_response is the number of bytes sent
	// It is used when we send requests to make sure all bytes are sent
	m_response = 0;

	// 404 handling
	if (this->is_valid_header ==  false) {

		int response = snprintf(m_resBuffer.get(), m_resBuffer_size, 
				 "HTTP/1.1 404 Not Found\r\n"
				 "Content-Type: text/html\r\n"
				 "Content-Length: 0\r\n"
				 "\r\n\r\n" );
		m_response = response;
		return;
	}

	
	// Adding 2 because we add \r\n after the payload
	size_t payload_size {strlen(file.get()) + 2};

	// Content-Length needs to match the size of the payload
	int response = snprintf(m_resBuffer.get(), m_resBuffer_size, 
			 "HTTP/1.1 200 OK\r\n"
			 "Content-Type: application/json\r\n"
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
	if (m_clientSocket <  0) { return;}
	// Here we need to send in a loop, send() could send the response partially
	// We make sure that every bytes is sent
	size_t rest_bytes = m_response;
	// Be careful to not delete or nullptr current, .get() only gives a temporary access
	// Current can be used if buffer_se is not reset or nullptr
	char* current = m_resBuffer.get();

	// We calculate X by taking the min value between rest bytes to send and the buffer size
	std::cout << "1 - Sending -- \n" << m_resBuffer.get() << '\n';
	size_t X = std::min(rest_bytes, m_resBuffer_size);
	ssize_t sendResponse = send(m_clientSocket, current, X, 0);
	rest_bytes -= sendResponse;
	current += sendResponse;
	if (sendResponse == -1) {
		current = nullptr;
		std::cerr << "Failed to send a response to the client." << '\n';
		return;
	}
	current = nullptr;
}


void Response::send_database() {
	if (m_clientSocket <  0) { return;}
	 std::cout << "Sending database to -- " << m_clientSocket << '\n';

	std::unique_ptr<char[]> file = get_file("public/database.json");

	// Adding 2 because we add \r\n after the payload
	size_t payload_size {strlen(file.get()) + 2};

	size_t send_buffer_size {4096};

	std::unique_ptr<char[]> send_buffer {new char[send_buffer_size]};
	std::memset(send_buffer.get(), 0, send_buffer_size);

	// Content-Length needs to match the size of the payload
	int response = snprintf(send_buffer.get(), send_buffer_size, 
			 "HTTP/1.1 200 OK\r\n"
			 "Content-Type: application/json\r\n"
			 "Content-Length: %d\r\n"
			 "\r\n"
			 "%s\r\n",
			 static_cast<int>(payload_size),
			 file.get());
	
	if (response >= static_cast<int>(send_buffer_size)) {
		std::cerr << "Response was truncated" << '\n';
		return;
	}
	else if (response < 0) {
		std::cerr << "Error formating response" << '\n';
		return;
	}

	// Here we need to send in a loop, send() could send the response partially
	// We make sure that every bytes is sent
	size_t rest_bytes = response;
	// Be careful to not delete or nullptr current, .get() only gives a temporary access
	// Current can be used if buffer_se is not reset or nullptr
	char* current = send_buffer.get();

	// We calculate X by taking the min value between rest bytes to send and the buffer size
	size_t X = std::min(rest_bytes, send_buffer_size);
	ssize_t sendResponse = send(m_clientSocket, current, X, 0);
	rest_bytes -= sendResponse;
	current += sendResponse;
	if (sendResponse == -1) {
		current = nullptr;
		std::cerr << "Failed to send a response to the client." << '\n';
		return;
	}
	current = nullptr;
	std::cout << "2 - Sending -- \n" << send_buffer.get() << '\n';
}
