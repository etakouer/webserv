#include "Master.hpp"

const std::string Master::_DEFAULT_CONF = "default.conf";

Master::Master(std::string const & file_conf) : _file_conf(file_conf), _servers(), _socket_servers() { 
}

void Master::init() {
	
	add_status_msgs();
	add_default_pages();
	add_mime_types();

	// choose file
	if (_file_conf.empty())
		_file_conf = _DEFAULT_CONF;

	std::cout << "Load file config ["<< _file_conf << "]\n";

	std::ifstream ifs (_file_conf);
	if(!ifs.is_open()) {
		std::cerr << "Error opening file config : " << strerror(errno) << "\n";
		exit(1);
	}
	Loader loader(ifs);
	try {
		loader.add_servers(_servers);
	} catch (std::exception const & e) {
		std::cerr << "Error : " << e.what() << "\n";
		exit(1);
	}
	if (_servers.empty()) {
		std::cerr << "Error : Any correct server in file config\n";
		exit(1);
	}

	//create socket_srevers

	std::set<std::pair<std::string, std::string> > listens;
	for (std::vector<Server>::const_iterator it = _servers.begin(); it != _servers.end(); ++it)
			listens.insert(std::make_pair(it->address, it->port));

	for (std::set<std::pair<std::string, std::string> >::const_iterator it = listens.begin(); it != listens.end(); ++it) {
		Socket_server socket_server(it->first, it->second);
		_add_servers(socket_server);
		try {
			socket_server.bind_();
		}catch(std::exception & e){
			std::cerr << "Error bind : " << e.what() << " [" <<socket_server.address << ":" << socket_server.port << "]\n";
			exit(1);
		}
		_socket_servers.insert(std::make_pair(socket_server.fd, socket_server));
	}
	
}

void Master::_add_servers(Socket_server & socket_server) {
	std::vector<Server>::iterator iter = _servers.begin();
	while (iter != _servers.end()) {
		if (iter->address == socket_server.address && iter->port == socket_server.port)
			socket_server.servers.push_back(&(*iter));
    	iter++;
	}
}


void Master::work() {
	std::cout << "Start webserv 1.20.1\n";
	std::cout << "Listen : ";
	for(std::map<int, Socket_server>::iterator it = _socket_servers.begin(); it !=_socket_servers.end(); it++) {
	std::cout << "[" << it->second.address << ":" << it->second.port << "] ";
		try {
			it->second.listen_();
		}catch(std::exception & e){
			std::cerr << "Error listen : " << e.what() << " [" << it->second.address << ":" << it->second.port << "]\n";
			exit(1);
		}
	}
	std::cout << "\n\n";
		try {
			Worker worker(_socket_servers);
			#ifdef DEBUG	
				what();
			#endif
			worker.event_loop(); 
		} 
		catch (std::exception & e) { 
			std::cerr << "kqueue/kevent error : " << e.what() << "\n";
		}
}

void Master::what() const {

	std::cout << "_DEFAULT_CONF: " << _DEFAULT_CONF << "\n";
	std::cout << "_file_conf: " << _file_conf << "\n";

	for_each(_servers.begin(), _servers.end(), _what_vector<Server>);
	for (std::map<int, Socket_server>::const_iterator it = _socket_servers.begin(); it !=_socket_servers.end(); it++)
		it->second.what();
}
