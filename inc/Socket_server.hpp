#pragma once

#include <iostream>
#include <fcntl.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <stdlib.h>
#include "utils.hpp"
#include "Server.hpp"

class Socket_server {

	public:
		int fd;
		std::string address;
		std::string port;
		static const int BACKLOG = 128;
		std::vector<Server *> servers;

	public:
		Socket_server(std::string const & address, std::string const & port);
		void listen_();
		void bind_();
		void what() const;
		void big_what() const;
};
