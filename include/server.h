#ifndef SERVER_H
#define SERVER_H

#include <unordered_map>

class RequestsHandler;

class Server {
	// This file allow me to setup what server.cpp can use (in term of functions and variables)
private:
	int m_port {};
	std::unordered_map<int, RequestsHandler*> m_con_list {};


public:
	int sockfd {};
	// constructor for the socket
	Server(int port) 
		: m_port {port}
	{
	}

	// basic fonction
	void start() ; 
	void stop();
	void connection();
	bool is_con_active(int& clientfd);
	RequestsHandler* get_active_con(int& clientfd);

};

#endif
