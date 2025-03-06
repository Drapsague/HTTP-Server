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
	int m_response {};
	size_t m_resBuffer_size {};

	std::unique_ptr<char[]> m_header {};
	std::unique_ptr<char[]> m_resBuffer {new char[m_resBuffer_size]};
	std::unique_ptr<char[]> m_recvBuffer {new char[m_resBuffer_size]};


public:
	// constructor for the request
	Response(RequestsHandler* con ,size_t resBuffer_size)
		: connection_ {con}, m_resBuffer_size {resBuffer_size}
	{
	}
	~Response() {}
	/*Response(int sockfd, int clientSocket, size_t resBuffer_size)*/
	/*	: m_sockfd{sockfd}, m_sockClient {clientSocket}, m_resBuffer_size {resBuffer_size}*/
	/*{*/
	/*}*/
	// basic fonction
	void recv_request();
	void parse_request();
	void create_response();
	void send_response();
	bool end_requests();
	
};

#endif
