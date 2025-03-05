#include "requestsHandler.h"
#include "request.h"
#include "response.h"

#include <cstdio>
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <unistd.h>
#include <cstring>

void RequestsHandler::createRequest() {
	/*std::cout << "A" << std::endl;*/
	m_req.recv_request();
	m_req.parse_request();
}

void RequestsHandler::createResponse() {
	/*std::cout << "B" << std::endl;*/
	m_res.create_response();
	m_res.send_response();
}

void RequestsHandler::handleClient() {
	std::cout << "Handling Client" << '\n';
	createRequest();
	createResponse();
}

