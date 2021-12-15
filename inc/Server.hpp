#pragma once

#include <string>
#include <iostream>
#include <map>
#include <vector>
#include <sstream>
#include <iomanip>
#include <exception>
#include "utils.hpp"
#include "Route.hpp"
#include "Request.hpp"

class Server
{
public:
	std::string					address;
	std::string					port;
	std::vector<std::string>	server_name;
	std::vector<Route>			routes;
	
public:
	Server();
	~Server();

	void add_route(const Route & route);
	Route choose_route(const std::string & uri);
	void what() const;

public:
	Route _find_default_route(std::string const & ext);
	void _fill_route_candidate(std::vector<std::string> & loc_tk, std::vector<Route> & locs_match, size_t & n_match);
	void _depth_count(std::vector<std::string> & loc_tk, size_t & n_match);
	void define_token_ext(std::vector<std::string> & tk, std::string & ext);
	std::string _delete_uri_variable(std::string & loc);
	void _format_uri(std::string & loc);
	void _remove_simple_dot(std::string & loc);
	bool _format_double_dot(std::string & loc);
	void _decode_uri(std::string & loc);
	void _delete_duplicate_slash(std::string & loc);
};

