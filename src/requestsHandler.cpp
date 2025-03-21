#include "../include/requestsHandler.h"
#include "../include/response.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <unistd.h>
#include <cstring>


/*RequestsHandler::RequestsHandler(Server* serv, int& clientSocket)*/
/*		:   m_serv {serv}, m_clientSocket {clientSocket}*/
/*{*/
/*	std::cout << "creating new RequestHandler instance for the socket : " << m_clientSocket << '\n';*/
/*}*/


void RequestsHandler::createResponse() {
	m_res = std::make_unique<Response>(this, 4096);
}

void RequestsHandler::handleClient(int& clientSockfd) {
	/*Response req {this, 4096};*/
	m_res->m_clientSocket = clientSockfd;

	std::cout << "Socket ---- " << m_res->m_clientSocket << '\n';
	m_res->recv_request();
	m_res->create_response();
	m_res->send_response();
}

