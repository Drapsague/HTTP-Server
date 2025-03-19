#include "../include/requestsHandler.h"
#include "../include/response.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <unistd.h>
#include <cstring>


void RequestsHandler::createResponse() {
	Response m_res {Response(this, 4096)};
	m_res.recv_request();
	m_res.create_response();
	m_res.send_response();

}

void RequestsHandler::handleClient() {
	createResponse();
}

