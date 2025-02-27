#ifndef REQUEST_H
#define REQUEST_H

#include <cstddef>
#include <cstdio>
#include <memory>

#include "server.h"

class Request {
	// This file allow me to setup what Request.cpp can use (in term of functions and variables)
private:
	Server* server_ {};
	int m_sockfd {server_->getSockClient()};
	int m_clientSock {server_->getSockClient()};
	size_t m_reqBuffer_size {};
	std::unique_ptr<char[]> m_reqBuffer {new char[m_reqBuffer_size]};
	std::unique_ptr<char[]> m_header {};

public:
	// constructor for the request
	Request(Server* server ,size_t reqBuffer_size)
		: server_ {server}, m_reqBuffer_size {reqBuffer_size}
	{
	}
	// basic fonction
	void recv_request();
	void parse_request();
	const char* get_header();



};

#endif
