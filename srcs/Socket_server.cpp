#include "Socket_server.hpp"

Socket_server::Socket_server(std::string const & address, std::string const & port) : fd(-1), address(address), port(port) {
}

void Socket_server::listen_() {
	int ret = listen(this->fd, this->BACKLOG);
	if (ret == -1)
		throw std::runtime_error(strerror(errno));
}

void Socket_server::bind_() {

	// create socket
	int ret = 0;
	this->fd = socket(PF_INET, SOCK_STREAM, 0);
	if (fd == -1)
		throw std::runtime_error(strerror(errno));

	// socket options
    ret = setsockopt(fd, SOL_SOCKET, SO_REUSEPORT, (int[]){1}, 4);
	if (ret == -1)
		throw std::runtime_error(strerror(errno));

	ret = fcntl(this->fd, F_SETFL, O_NONBLOCK);
	if (ret == -1)
		throw std::runtime_error(strerror(errno));

	// bind
	struct sockaddr_in address;
	bzero(&address, sizeof(address));
	address.sin_family = AF_INET;
	address.sin_port = htons(atoi(this->port.c_str()));
	inet_pton(AF_INET, this->address.c_str(), &(address.sin_addr));

	ret = bind(this->fd, (struct sockaddr *)&address, sizeof(address));
	if (ret == -1) 
		throw std::runtime_error(strerror(errno));
}

void Socket_server::what() const {
	std::cout << "+ Socket_server : " << "\n";
	std::cout << "- fd: " << this->fd << "\n";
	std::cout << "- address: " << this->address << "\n";
	std::cout << "- port: " << this->port << "\n";
}

void Socket_server::big_what() const {
	what();
	for (std::vector<Server *>::const_iterator it = servers.begin(); it !=servers.end(); it++)
		(*it)->what();
}
