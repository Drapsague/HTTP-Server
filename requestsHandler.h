#ifndef CLIENTH_H
#define CLIENTH_H

#include <cstddef>
#include <cstdio>
#include <memory>

#include "server.h"
#include "request.h"
#include "response.h"

class RequestsHandler {
	// This file allow me to setup what RequestsHandler.cpp can use (in term of functions and variables)
private:
	Server* server_ {};
	int m_sockfd {server_->getSockfd()};
	int m_clientSocket {server_->getSockClient()};
	size_t m_sendBuffer_size {};
	std::unique_ptr<char[]> m_sendBuffer {new char[m_sendBuffer_size]};
	Request m_req {Request(server_, 4096)};
	Response m_res {Response(server_, 4096)};
	

public:
	// constructor for the socket
	/*RequestsHandler(int sockfd, int clientSocket, size_t sendBuffer_size)*/
	/*	: m_sockfd {sockfd}, m_clientSocket {clientSocket}, m_sendBuffer_size {sendBuffer_size}*/
	/*{*/
	/*}*/
	RequestsHandler(Server* server, size_t sendBuffer_size)
		: server_ {server}, m_sendBuffer_size {sendBuffer_size}
	{
	}
	// basic fonction
	void setBuffer(size_t buffer_size) {m_sendBuffer_size = buffer_size;}
	void recvRequest();
	int parseRequest();
	void sendResponse();

	void createRequest();
	void createResponse();
	void handleClient();



};

#endif
