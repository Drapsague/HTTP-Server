#ifndef CLIENTH_H
#define CLIENTH_H

#include "server.h"
#include "response.h"

#include <sys/socket.h>
#include <cstddef>
#include <cstdio>
#include <memory>
#include <iostream>

class Response;

class RequestsHandler {
	// This file allow me to setup what RequestsHandler.cpp can use (in term of functions and variables)
private:
	std::unique_ptr<Response> m_res {};

public:
	RequestsHandler(Server* serv, int& clientSocket)
			:   m_serv {serv}, m_clientSocket {clientSocket}
	{
		std::cout << "creating new RequestHandler instance for the socket : " << m_clientSocket << '\n';
	}
	Server* m_serv {};
	int m_clientSocket {};
	// constructor for the socket
	
	/*RequestsHandler(Server* serv, int& clientSocket);*/
	/*~RequestsHandler() {}*/
	// basic fonction
	void sendResponse();
	void createResponse();
	void handleClient();
};

#endif
