#include "../include/requestsHandler.h"
#include "../include/response.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <unistd.h>
#include <cstring>
#include <memory>


/*RequestsHandler::RequestsHandler(Server* serv, int& clientSocket)*/
/*		:   m_serv {serv}, m_clientSocket {clientSocket}*/
/*{*/
/*	std::cout << "creating new RequestHandler instance for the socket : " << m_clientSocket << '\n';*/
/*}*/


void RequestsHandler::createResponse(int& clientSockfd) {
	m_res = std::make_shared<Response>(clientSockfd);

}

void RequestsHandler::handleClient(std::shared_ptr<Response> res_ptr) {
	res_ptr->recv_request();
	// res_ptr->create_response();
	// res_ptr->send_response();
}

void RequestsHandler::sendDatabase(std::shared_ptr<Response> res_ptr, std::shared_ptr<char[]> last_message) {
	std::shared_ptr<char[]> message = last_message;
	res_ptr->send_database(message);
}

std::unique_ptr<char[]> RequestsHandler::get_last_message(std::shared_ptr<Response> res_ptr) {
	std::unique_ptr<char[]> last_message = res_ptr->get_serialized_content();
	return last_message;

}
