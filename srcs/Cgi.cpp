#include "Cgi.hpp"

std::string Cgi::server_protocol = "SERVER_PROTOCOL=HTTP/1.1";
std::string Cgi::request_scheme = "REQUEST_SCHEME=http";
std::string Cgi::gateway_interface = "GATEWAY_INTERFACE=CGI/1.1";
std::string Cgi::server_software = "SERVER_SOFTWARE=webserv/v2";

Cgi::Cgi(void) : pid(-1)
{
	;
}

Cgi::~Cgi(void)
{	
	;
}

void Cgi::close_pipe_worker_side(void)
{
	close(input[0]);
	close(output[1]);
}

void Cgi::close_pipe_cgi_side(void)
{
	close(input[1]);
	close(output[0]);
}

void Cgi::what(void) const
{
	;
}
