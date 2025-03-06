#include "requestsHandler.h"
#include "response.h"

#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <unistd.h>
#include <cstring>

/*void RequestsHandler::createRequest() {*/
/*	std::cout << "A" << std::endl;*/
/*	m_req.recv_request();*/
/*	m_req.parse_request();*/
/*}*/

void RequestsHandler::createResponse() {
	/*std::cout << "B" << std::endl;*/
	/*Response m_res {Response(m_serv->sockfd, *m_clientSocket, 4096)};*/
	Response m_res {Response(this, 4096)};
	while(m_clientSocket > 0) {
		m_res.recv_request();
		m_res.parse_request();
		m_res.create_response();
		m_res.send_response();
	}

}

void RequestsHandler::handleClient() {
	std::cout << "Handling Client" << '\n';
	/*createRequest();*/
	createResponse();
}

