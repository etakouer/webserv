#include "Server.hpp"
#include "Request.hpp"
#include <cstring>
#include <sstream>
#include <iomanip>

Server::Server() 
{
	address = "0.0.0.0";
	server_name.push_back("localhost");
	port = "8080";
	Route newroute;
	routes.push_back(newroute);
}

Server::~Server()
{
	;
}

void	Server::add_route(const Route & route)
{
	std::vector<Route>::iterator it = routes.begin();
	std::vector<Route>::iterator ite = routes.end();
	while (it != ite)
	{
		if (!route.location.compare(it->location) && !route.ext.compare(it->ext))
			throw std::runtime_error("Location duplica");
		it++;
	}
	routes.push_back(route);
}

void	Server::_remove_simple_dot(std::string & loc)
{
	size_t n = 0;
	for (int i = 0; (n = loc.find("./", i)) && n != std::string::npos; i++)
		if ((n - 1 >= 0 && loc[n - 1] != '.'))
			loc.erase(n, 2);
}

bool	Server::_format_double_dot(std::string & loc)
{
	size_t n = 0;
	size_t n_save = 0;
	while ((n = loc.find("/..")) != std::string::npos)
	{
		loc.erase(n, 3);
		n_save = n;
		if (!n--)
			return 0;
		if ((n = loc.rfind("/", n)) == std::string::npos)
			return 0;
		loc.erase(n, n_save - n);
	}
	return 1;
}

std::string	Server::_delete_uri_variable(std::string & loc)
{
	size_t n = loc.find('?');
	std::string query = "";
	if (n != std::string::npos) {
		query = loc.substr(n + 1, std::string::npos);
		loc.erase(n, loc.size() - n);
	}
	return query;
}

void	Server::_decode_uri(std::string & loc)
{
	std::ostringstream ss;
	std::string buf;
	size_t n = 0;

	while ((n = loc.find('%')) && n != std::string::npos && n < loc.size())
	{
		ss << static_cast<char>(HexToInt<int>(loc.substr(n + 1, 2)));
		loc.erase(n, 3);
		loc.insert(n, ss.str());
		ss.str("");
		ss.clear();
		n++;
	}
}

void	Server::_delete_duplicate_slash(std::string & loc)
{
	size_t n = 0;
	while ((n = loc.find("//")) != std::string::npos)
		loc.erase(n, 1);
}

void	Server::define_token_ext(std::vector<std::string> & tk, std::string & ext)
{
	std::vector<std::string>::iterator ite_loc = tk.end();
	size_t n;
	if (ite_loc == tk.begin())
		return ;
	if ((n = (ite_loc - 1)->rfind(".")) != std::string::npos)
		ext = (ite_loc - 1)->substr(n, (ite_loc - 1)->size() - 1).erase(0,1);
}

void	Server::_depth_count(std::vector<std::string> & loc_tk, size_t & n_match)
{
	std::vector<Route>::iterator it = routes.begin();
	std::vector<Route>::iterator ite = routes.end();
	std::vector<std::string>::iterator it_loc;
	std::vector<std::string>::iterator ite_loc;
	std::vector<std::string> routes_tk;
	std::vector<std::string>::iterator it_route;
	std::vector<std::string>::iterator ite_route;
	std::string loc_ext;
	size_t tmpn_match = 0;
	while (it != ite)
	{
		routes_tk = split(it->location, '/');
		it_route = routes_tk.begin();
		ite_route = routes_tk.end();
		it_loc = loc_tk.begin();
		ite_loc = loc_tk.end();
		tmpn_match = 0;
		while (it_route != ite_route && it_loc != ite_loc)
		{
			if (*it_route++ != *it_loc++)
			{
				tmpn_match = 0;
				break;
			}		
			tmpn_match++;
		}
		if (tmpn_match > n_match)
			n_match = tmpn_match;
		it++;
	}
}

void	Server::_fill_route_candidate(std::vector<std::string> & loc_tk, std::vector<Route> & locs_match, size_t & n_match)
{
	std::vector<Route>::iterator it = routes.begin();
	std::vector<Route>::iterator ite = routes.end();
	std::vector<std::string>::iterator it_loc;
	std::vector<std::string>::iterator ite_loc;
	std::vector<std::string> routes_tk;
	std::vector<std::string>::iterator it_route;
	std::vector<std::string>::iterator ite_route;
	std::string loc_ext;
	size_t tmpn_match = 0;
	while (it != ite)
	{
		routes_tk = split(it->location, '/');
		it_route = routes_tk.begin();
		ite_route = routes_tk.end();
		it_loc = loc_tk.begin();
		ite_loc = loc_tk.end();
		tmpn_match = 0;
		while ((it_route != ite_route) && (it_loc != ite_loc))
		{
			if (*it_route++ != *it_loc++)
			{
				tmpn_match = 0;
				break;
			}
			tmpn_match++;
		}
		if (tmpn_match == n_match)
			locs_match.push_back(*it);
		it++;
	}
}

Route	Server::_find_default_route(std::string const & ext)
{
	std::vector<Route>::iterator it = routes.begin();
	std::vector<Route>::iterator ite = routes.end();

	while (it != ite)
	{
		if (!it->location.empty() && !it->location.compare("/") && !it->ext.compare(ext))
			return *it;
		it++;
	}
	it = routes.begin();
	while (it != ite)
	{
		if (it->location.empty() && !it->ext.compare(ext))
			return *it;
		it++;
	}
	it = routes.begin();
	while (it != ite)
	{
		if (!it->location.empty() && !it->location.compare("/"))
			return *it;
		it++;
	}
	return routes[0];
}

Route	Server::choose_route(const std::string & uri)
{
	std::string loc(uri);
	std::vector<Route>::iterator it = routes.begin();
	std::vector<Route>::iterator ite = routes.end();
	std::vector<std::string> loc_tk(split(loc, '/'));
	std::vector<std::string> routes_tk;
	std::vector<std::string>::iterator it_route;
	std::vector<std::string>::iterator ite_route;
	std::vector<Route> locs_match;
	std::string loc_ext;
	size_t n_match = 0;

	define_token_ext(loc_tk, loc_ext);
	Route best_match = routes.at(0);
	_depth_count(loc_tk, n_match);
	if (!n_match)
		return _find_default_route(loc_ext);
	_fill_route_candidate(loc_tk, locs_match, n_match);
	it = locs_match.begin();
	ite = locs_match.end();
	while (it != ite)
	{
		if (!it->ext.compare(loc_ext))
			return *it;
		else if (it->ext.empty())
			best_match = *it;
		it++;
	}

	return best_match;
}

void	Server::what() const
{
	std::cout << "------ Server ------" << std::endl;
	std::vector<Route>::const_iterator it = routes.begin();
	std::vector<Route>::const_iterator ite = routes.end();
	std::vector<std::string>::const_iterator it_serv = server_name.begin();

	std::cout << "address : " << address << std::endl;
	std::cout << "port : " << port << std::endl;
	std::cout << "server_name : " << std::endl;
	for (;it_serv != server_name.end(); ++it_serv)
		std::cout << "	" << *it_serv << std::endl;
	while (it != ite)
	{
		it.base()->what();
		it++;
	}
}
