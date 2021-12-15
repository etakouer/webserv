#pragma once
#include <unistd.h>
#include <vector>
#include <iostream>
#include <string>

class Cgi {

	public:

		std::string					path;
		std::string					pwd;
		std::string					query_string;
		std::string					request_method;
		std::string					content_type;
		std::string					content_length;
		std::string					path_translated;
		std::string					request_uri;
		std::string					document_uri;
		static std::string	server_protocol;
		static std::string	request_scheme;
		static std::string	gateway_interface;
		static std::string	server_software;
		std::string					remote_addr;
		std::string					remote_port;
		std::string					server_port;
		std::string					server_name;
		std::string					script_name;
		std::string					script_filename;
		std::string					path_info;
		std::string					http_cookie;
		std::vector<std::string>	special_headers;

		pid_t						pid;
		int							input[2];
		int							output[2];
		int							exit_code;
		std::vector<char *>			envp;

		Cgi(void);
		~Cgi(void);
		void close_pipe_worker_side(void);
		void close_pipe_cgi_side(void);
		void what(void) const;
};

