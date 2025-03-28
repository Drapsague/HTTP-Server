#ifndef CLIENT_H
#define CLIENT_H

#include <memory>
#include <iostream>


class Client {
	// This file allow me to setup what client.cpp can use (in term of functions and variables)
private:
	int m_port {};
	int m_response {};

	const size_t m_resBuffer_size {};

	std::unique_ptr<char[]> m_header {};

	std::unique_ptr<char[]> m_resBuffer {};
	std::unique_ptr<char[]> m_recvBuffer {};
	std::unique_ptr<char[]> m_payload {};
	

public:
	int m_sockfd {};
	// constructor for the socket
	Client(int port, size_t buffer_size) 
		: m_port {port}, m_resBuffer_size {buffer_size}
	{
		m_resBuffer = std::make_unique<char[]>(m_resBuffer_size);
		m_recvBuffer = std::make_unique<char[]>(m_resBuffer_size);
		m_payload = std::make_unique<char[]>(m_resBuffer_size);
	}


	// basic fonction
	void connection();
	void create_response();
	void send_response();
	ssize_t recv_request();
	void response();
	void receive();

};

#endif
