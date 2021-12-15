#include "Loader.hpp"

Loader::Loader(std::ifstream& config) : _config_file(config)
{
#ifdef DEBUG 
	std::cout << "Loader start" << std::endl;
#endif
}

Loader::~Loader(void)
{
#ifdef DEBUG 
	std::cout << "Loader destruction" << std::endl;
#endif
}

void	Loader::add_servers(std::vector<Server> & servers)
{
	_parse_config(servers);
}

/* Fill a vector of string with a pair (line + line number)  */
void	Loader::_fill_config_tab(void)
{
	std::string		line;
	unsigned int	nb = 0;
	while (getline(_config_file, line))
	{
		++nb;
		/* Trim spaces/tab and ignore empty lines or commented lines */
		line = _trim(line);
		if (line.empty() || line[0] == '#')
			continue ;
		_config_tab.push_back(std::make_pair(line, nb));
	}
}

void	Loader::_parse_config(std::vector<Server> & servers)
{
	t_vector_pair_iterator	beg;
	
	_fill_config_tab();	
	/* Iterate through the vector of pair */
	for (beg = _config_tab.begin(); beg != _config_tab.end(); beg++)
	{
		/* Create new server, new route and vector of iterator to save position of locations
		 * Treat the server directives
		 * Add the route to the server
		 * Call _create_route to iterate through the vector of iterator to treat locations directives
		 * Add the new server to the vector of servers */
		if ((beg->first).find("server") != std::string::npos)
		{
			if ((beg->first)[(beg->first).size() - 1] != '{'
					&& ((++beg)->first).compare("{") != 0)
				SYNTAX_ERROR(beg->second, beg->first);
			++beg;

			Server				new_server;
			Route				default_route;
			t_vector_iterator	loc_it;
			while (beg != _config_tab.end() && ((beg->first).compare("}")) != 0)
			{
				_treatment_server(beg, new_server, default_route, loc_it);
				if (beg + 1 != _config_tab.end())
					++beg;
				else
					SYNTAX_ERROR(beg->second, beg->first);
			}
			new_server.routes.clear();
			new_server.add_route(default_route);
			_create_route(new_server, default_route, loc_it);
			_add_server_to_vector(servers, new_server);
		}
	}
}

/* Add the new server to the vector + check duplicates */
void	Loader::_add_server_to_vector(std::vector<Server>& servers, const Server& new_server)
{
	std::vector<Server>::iterator	it = servers.begin();
	for (; it != servers.end(); ++it)
	{
		if ((*it).address.compare(new_server.address) == 0)
		{
			if ((*it).port.compare(new_server.port) == 0)
			{
				t_vector_string_iterator it_serv = (*it).server_name.begin();
				std::vector<std::string>::const_iterator it_new_serv = new_server.server_name.begin();
				for (; it_new_serv != new_server.server_name.end(); ++it_new_serv)
				{
					for (it_serv = (*it).server_name.begin(); it_serv != (*it).server_name.end(); ++it_serv)
					{
						if ((*it_new_serv).compare(*it_serv) == 0)
						{
							throw std::runtime_error("Configuration error: Two same servers");
						}
					}
				}
			}
		}
	}
	servers.push_back(new_server);
}

/* Iterate through the vector of iterator to create all routes */
void	Loader::_create_route(Server & server, Route &default_route, t_vector_iterator& loc_it)
{
	for (t_vector_iterator::iterator beg = loc_it.begin(); beg != loc_it.end(); ++beg)
	{
		Route 						new_route(default_route);
		t_vector_string tmp_split;
		if (((*beg)->first).find("\"") != std::string::npos)
		{
			t_vector_string	tmp_split_quote = split((*beg)->first, '"');
			tmp_split = split(tmp_split_quote.at(0), ' ');
			tmp_split.push_back(tmp_split_quote.at(1));
		} else 
			tmp_split = split((*beg)->first, ' ');

		t_vector_pair_iterator	line = ++(*beg);
		t_vector_iterator			 new_loc_vec;
		_get_location_name(tmp_split, (*beg)->second, new_route);
		/* Treat each location */
		while ((line->first).compare("}") != 0)
		{
			_treatment_location(line, new_route, new_loc_vec);
			if ((line + 1) != _config_tab.end())
				++(line);
			else
				SYNTAX_ERROR(line->second, line->first);
		}
		/* Check if there is a cgi in an extension route */
		if (new_route.ext.size() && !new_route.cgi.size())
		{
			std::string tmp_error = "Configuration error at line ";
			tmp_error +=  to_string(line->second); 
			tmp_error +=  " no CGI script in an extension location";
			throw std::runtime_error(tmp_error);
		}
		server.add_route(new_route);
		_create_route(server, new_route, new_loc_vec);
	}
}

std::string Loader::_ltrim(const std::string &s)
{
    size_t start = s.find_first_not_of(" \r\t");
    return (start == std::string::npos) ? "" : s.substr(start);
}
 
std::string Loader::_rtrim(const std::string &s)
{
    size_t end = s.find_last_not_of(" \r\t");
    return (end == std::string::npos) ? "" : s.substr(0, end + 1);
}

std::string	Loader::_trim_comments(const std::string &s)
{
	size_t	pos = s.find("#");
	return (pos == std::string::npos) ? s : s.substr(0, pos);
}
 
std::string Loader::_trim(const std::string &s) {
    return _rtrim(_ltrim(_trim_comments(s)));
}

void	Loader::_print_config(std::pair<std::string, unsigned int> curr_line) const {
	std::cout << curr_line.first << std::endl;
}

void	Loader::what(void) const
{
	std::string tmp;

	std::cout << "Loader content : " << std::endl;
	for_each(_config_tab.begin(), _config_tab.end(), std::bind1st(std::mem_fun(&Loader::_print_config), this));
}
