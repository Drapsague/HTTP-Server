#include "request.h"

#include <cstddef>
#include <cstdio>
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <unistd.h>
#include <cstring>
#include <memory>

void Request::recv_request() {
	// Receiving incomming requests
	ssize_t rec = recv(m_clientSock, m_reqBuffer.get(), m_reqBuffer_size, 0);
	/*std::cout << "Mesage received" << std::endl;*/
	if (rec <= 0) {
		std::cerr << "Client disconnected or an error occured" << std::endl;
		close(m_sockfd);
		close(m_clientSock);
		/*throw "Client disconnected or an error occured";*/
		return;
	}
	std::cout << m_reqBuffer.get() << std::endl;
}

void Request::parse_request() {
	/*std::cout << "Mesage parse" << std::endl;*/
	// Here we retreive the string after the end of the first line
	// We get the size of that string and substract it to the size of the whole request
	char* body_request {strstr(m_reqBuffer.get(), "\r\n")};
	size_t fline_size  {strlen(m_reqBuffer.get()) - strlen(body_request)};

	// Creating a string with the exact size of the first line
	// And copying the first line in that string
	// Assigning the header to our class argument
	m_header = std::make_unique<char[]>(fline_size + 1);
	strncpy(m_header.get(), m_reqBuffer.get(), fline_size);
	m_header.get()[fline_size] = '\0';

	std::cout << m_header.get() << std::endl;

}

const char* Request::get_header() {return m_header.get();}

