#ifndef RESPONSE_H
#define RESPONSE_H

#include <cstddef>
#include <cstdio>
#include <memory>

#include "server.h"

class Response {
	// This file allow me to setup what Response.cpp can use (in term of functions and variables)
private:
	Server* server_ {};
	int m_sockfd {server_->getSockfd()};
	int m_sockClient {server_->getSockClient()};
	int m_response {};
	size_t m_resBuffer_size {};
	std::unique_ptr<char[]> m_resBuffer {new char[m_resBuffer_size]};


public:
	// constructor for the request
	Response(Server* server, size_t resBuffer_size)
		: server_ {server}, m_resBuffer_size {resBuffer_size}
	{
	}
	// basic fonction
	void create_response();
	void send_response();
	
};

#endif
