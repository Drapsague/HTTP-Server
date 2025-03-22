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
	/*std::unique_ptr<Response> m_res {};*/

public:
	RequestsHandler(Server* serv)
			:   m_serv {serv}
	{
	}
	Server* m_serv {};
	std::shared_ptr<Response> m_res {};
	// constructor for the socket
	
	/*~RequestsHandler() {}*/
	// basic fonction
	void sendResponse();
	void createResponse(int& clientfd);
	void handleClient(std::shared_ptr<Response>);
};

#endif
