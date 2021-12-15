#pragma once

#include <fstream>
#include <vector>
#include <algorithm>
#include <iostream>
#include <unistd.h>
#include "utils.hpp"
#include "Server.hpp"
#include "Route.hpp"

#define SYNTAX_ERROR(LINE, C) syntax_error(LINE, C);

class Loader {

public:
	typedef	std::vector<std::string>						t_vector_string;
	typedef std::vector< std::pair<std::string, unsigned int> >			t_vector_pair;
	typedef std::vector< std::pair<std::string, unsigned int> >::iterator			t_vector_pair_iterator;
	typedef std::vector< std::vector<std::pair<std::string, unsigned int> >::iterator> t_vector_iterator;
	typedef	std::vector<std::string>::iterator				t_vector_string_iterator;

public:
	Loader(std::ifstream & config);
	~Loader(void);

	void	what(void) const;
	void	add_servers(std::vector<Server> & servers);

private:
	std::ifstream & _config_file; 
	t_vector_pair _config_tab;

	Loader(void);
	void	_parse_config(std::vector<Server> & servers);
	void	_print_config(std::pair<std::string, unsigned int> curr_line) const;
	void	_create_route(Server & server, Route &default_route, t_vector_iterator& loc_it);	
	void	_fill_config_tab(void);
	void	_add_server_to_vector(std::vector<Server>& servers, const Server& new_server);
	
	void	_check_ip(const std::string& ip, unsigned int line);

	void	_treat_listen(t_vector_string split_line, unsigned int line, Server &new_server);
	void	_treat_server_name(t_vector_string split_line, unsigned int line,  Server &new_server);

	void	_treat_root(t_vector_string split_line, unsigned int line, Route &default_route);
	void	_treat_autoindex(t_vector_string split_line, unsigned int line, Route &default_route);
	void	_treat_max_body_size(t_vector_string split_line, unsigned int line, Route &default_route);
	void	_treat_return(t_vector_string split_line, unsigned int line, Route &default_route);
	void	_treat_index(t_vector_string split_line, unsigned int line, Route &default_route);
	void	_treat_limit_except(t_vector_string split_line, unsigned int line, Route &default_route);
	void	_treat_cgi(t_vector_string split_line, unsigned int line, Route &default_route);
	void	_treat_upload(t_vector_string split_line, unsigned int line, Route &default_route);
	void	_treat_error_page(t_vector_string split_line, unsigned int line, Route &default_route);
	void	_get_location_name(t_vector_string &split_line, unsigned int line, Route &default_route);

	void	_treat_location(t_vector_iterator &location_iterator, t_vector_pair_iterator &it);

	void	_treatment_server(t_vector_pair_iterator &begin, Server &new_server, Route &default_route, t_vector_iterator &location_iterator);
	void	_treatment_location(t_vector_pair_iterator &begin, Route &default_route, t_vector_iterator &location_iterator);
	void	_treatment_common(t_vector_string split_line, t_vector_pair_iterator &begin, Route &default_route, t_vector_iterator &location_iterator);
	
	std::string	_trim(const std::string &s);
	std::string _trim_comments(const std::string &s);
	std::string	_ltrim(const std::string &s);
	std::string	_rtrim(const std::string &s);
};

