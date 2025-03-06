#ifndef REQUEST_H
#define REQUEST_H

#include <cstddef>
#include <cstdio>
#include <memory>

/*#include "requestsHandler.h"*/

class Request {
	// This file allow me to setup what Request.cpp can use (in term of functions and variables)
private:
	/*RequestsHandler* connection_ {};*/

	int m_sockfd {};
	int m_clientSock {};
	size_t m_reqBuffer_size {};
	std::unique_ptr<char[]> m_reqBuffer {new char[m_reqBuffer_size]};
	std::unique_ptr<char[]> m_header {};

public:
	// constructor for the request
	/*Request(RequestsHandler* con ,size_t reqBuffer_size)*/
	/*	: connection_ {con}, m_reqBuffer_size {reqBuffer_size}*/
	/*{*/
	/*}*/
	Request(size_t reqBuffer_size)
		:  m_reqBuffer_size {reqBuffer_size}
	{
	}
	// basic fonction
	void recv_request();
	void parse_request();
	const char* get_header();



};

#endif
