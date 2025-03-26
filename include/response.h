#ifndef RESPONSE_H
#define RESPONSE_H

#include <cstddef>
#include <cstdio>
#include <memory>
#include <iostream>

class RequestsHandler;

class Response {
	// This file allow me to setup what Response.cpp can use (in term of functions and variables)
private:
	const size_t m_recvBuffer_size {4096};
	const size_t m_resBuffer_size {4096};
	bool is_valid_header {};
	int m_response {};

	std::unique_ptr<char[]> m_resBuffer {new char[m_recvBuffer_size]};
	std::unique_ptr<char[]> m_recvBuffer {new char[m_recvBuffer_size]};


public:
	int m_clientSocket {};
	int m_count;

	// constructor for the request
	Response(int& clientfd);
	~Response() 
	{
		std::cout << "Destroying Response instance : " << m_count << '\n';
	}

	// basic fonction
	void recv_request();
	std::unique_ptr<char[]> get_header_file();
	std::unique_ptr<char[]> get_content();
	void create_response();
	void send_response();
	bool end_requests();
	std::unique_ptr<char[]> get_file(char* header_ptr);
	
};

#endif
