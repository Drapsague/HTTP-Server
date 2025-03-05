#ifndef RESPONSE_H
#define RESPONSE_H

#include <cstddef>
#include <cstdio>
#include <memory>

#include "requestsHandler.h"

class Response {
	// This file allow me to setup what Response.cpp can use (in term of functions and variables)
private:
	RequestsHandler* connection_ {};
	int m_sockfd {};
	int m_sockClient {};
	int m_response {};
	size_t m_resBuffer_size {};
	std::unique_ptr<char[]> m_resBuffer {new char[m_resBuffer_size]};


public:
	// constructor for the request
	Response(RequestsHandler* con ,size_t resBuffer_size)
		: connection_ {con}, m_resBuffer_size {resBuffer_size}
	{
	}
	// basic fonction
	void create_response();
	void send_response();
	
};

#endif
