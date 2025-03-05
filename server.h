#ifndef SERVER_H
#define SERVER_H


class Server {
	// This file allow me to setup what server.cpp can use (in term of functions and variables)
private:
	int m_port {};
	int sockfd {};

public:
	// constructor for the socket
	Server(int port) 
		: m_port {port}
	{
	}

	// basic fonction
	void start() ; 
	const int& getSockfd() {return sockfd;}

};

#endif
