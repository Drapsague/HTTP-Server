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
	RequestsHandler* connection_ {};
	int m_response {};
	const size_t m_recvBuffer_size {};
	const size_t m_resBuffer_size {4096};
	bool is_valid_header {};

	std::unique_ptr<char[]> m_resBuffer {new char[m_recvBuffer_size]};
	std::unique_ptr<char[]> m_recvBuffer {new char[m_recvBuffer_size]};


public:
	// constructor for the request
	Response(RequestsHandler* con ,size_t recvBuffer_size);
	/*~Response() {}*/

	// basic fonction
	void recv_request();
	std::unique_ptr<char[]> get_header_file();
	void create_response();
	void send_response();
	bool end_requests();
	std::unique_ptr<char[]> get_file(char* header_ptr);
	
};

#endif
