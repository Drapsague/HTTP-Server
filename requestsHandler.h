#ifndef CLIENTH_H
#define CLIENTH_H

#include <sys/socket.h>
#include <cstddef>
#include <cstdio>
#include <memory>
#include <iostream>

#include "server.h"
/*#include "response.h"*/

class RequestsHandler {
	// This file allow me to setup what RequestsHandler.cpp can use (in term of functions and variables)
private:
	size_t m_sendBuffer_size {};
	std::unique_ptr<char[]> m_sendBuffer {new char[m_sendBuffer_size]};
	/*Request m_req {Request(this, 4096)};*/
	/*Response m_res {Response(this, 4096)};*/
	/*Response m_res {Response(m_serv->sockfd, *m_clientSocket, 4096)};*/
	

public:
	Server* m_serv {};
	int m_clientSocket {};
	// constructor for the socket
	/*RequestsHandler(int sockfd, int clientSocket, size_t sendBuffer_size)*/
	/*	: m_sockfd {sockfd}, m_clientSocket {clientSocket}, m_sendBuffer_size {sendBuffer_size}*/
	/*{*/
	/*}*/
	RequestsHandler(Server* serv, int& clientSocket, size_t sendBuffer_size)
		:  m_sendBuffer_size {sendBuffer_size}, m_serv {serv}, m_clientSocket {clientSocket}
	{
	}
	/*~RequestsHandler() {*/
	/*	std::cout << "Desctructing RequestHandler instance" << '\n';*/
	/**/
	/*}*/
	// basic fonction
	void setBuffer(size_t buffer_size) {m_sendBuffer_size = buffer_size;}
	void sendResponse();

	void createResponse();
	void handleClient();



};

#endif
