#pragma once
#include <iostream>
#include <string>
#include <set>
#include <map>
#include <vector>
#include <stdexcept>
#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <unistd.h>
#include <algorithm>
#include "Socket_server.hpp"
#include "Socket_client.hpp"
#include <signal.h>

class Worker {

	public:

	Worker(const std::map<int, Socket_server> & socket_servers);
	Worker(const Worker & ref);
	~Worker(void);
	Worker & operator=(const Worker & right);

	void event_loop(void);

	private:

	void	register_socket_servers(void);
	void	update_modif_list(int ident, int16_t filter,
				uint16_t flags = 0, uint32_t fflags = 0,
				intptr_t data = 0, void *udata = 0);

	void	new_client(int i);
	void	del_client(int i, Socket_client * client_ptr);

	void	recv_client(int i, Socket_client * client_ptr);
	void	send_client(int i, Socket_client * client_ptr);

	void	read_client(int i, Socket_client * client_ptr);
	void	write_client(int i, Socket_client * client_ptr);

	void	process_client(Socket_client & client);

	Socket_client * get_client(int i);

	void	abort_write(Socket_client & client);
	
	std::vector<struct kevent>		_event_list;
	std::vector<struct kevent>		_modif_list;
	int								number_of_events;
	std::map<int, Socket_client>	_socket_clients;
	std::map<int, Socket_server>	_socket_servers;
	std::set<int>					_closed_clients;

};
