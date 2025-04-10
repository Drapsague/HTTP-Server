#ifndef CLIENT_H
#define CLIENT_H

#include <string>  // for char_traits, operator+, string, basic_string
#include <memory>
#include <iostream>
#include <thread>
#include "ftxui/dom/elements.hpp"  // for text, hbox, separator, Element, operator|, vbox, border
#include "ftxui/component/component.hpp"       // for Input, Renderer, Vertical
#include "ftxui/component/component_base.hpp"  // for ComponentBase
#include "ftxui/component/component_options.hpp"  // for InputOption
#include "ftxui/component/screen_interactive.hpp"  // for Component, ScreenInteractive
#include "ftxui/dom/elements.hpp"  // for text, hbox, separator, Element, operator|, vbox, border
#include "ftxui/util/ref.hpp"  // for Ref

struct MessageOption {
	std::string username;
	std::string message;
	bool is_me;
	ftxui::Color color;
};

class Client {
	// This file allow me to setup what client.cpp can use (in term of functions and variables)
private:
	int m_port {};
	int m_response {};

	const size_t m_resBuffer_size {};

	std::unique_ptr<char[]> m_header {};

	
public:
	std::unique_ptr<char[]> m_resBuffer {};
	std::unique_ptr<char[]> m_recvBuffer {};
	std::unique_ptr<char[]> m_payload {};
	std::thread recv_thread {};
	std::string recv_message {};
	std::string m_username {};
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
	void create_response(std::string message);
	void send_response();
	ssize_t recv_request();
	void response();
	void receive();


};

#endif
