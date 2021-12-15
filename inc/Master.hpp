#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <set>
#include <map>
#include "Socket_server.hpp"
#include "Server.hpp"
#include "Loader.hpp"
#include "Worker.hpp"
#include "utils.hpp"

class Master {
	private:
		static const std::string _DEFAULT_CONF;
		std::string _file_conf;
		std::vector<Server> _servers;
		std::map<int, Socket_server> _socket_servers;

	public:
		Master(std::string const & file_config);
		void init();
		void work();
		void what() const;
	private:
		static void listen_(std::pair<int, Socket_server> & ss);
		void _add_servers(Socket_server & socket_server);
};
