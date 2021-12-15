#include "Loader.hpp"

/* Treat directives in location or server context */
void	Loader::_treatment_common(t_vector_string split_line, t_vector_pair_iterator &begin, Route &default_route, t_vector_iterator &location_iterator)
{
	if (split_line.at(0).compare("root") == 0)
		_treat_root(split_line, begin->second, default_route);
	else if (split_line.at(0).compare("autoindex") == 0)
		_treat_autoindex(split_line, begin->second, default_route);
	else if (split_line.at(0).compare("client_max_body_size") == 0)
		_treat_max_body_size(split_line, begin->second, default_route);
	else if (split_line.at(0).compare("return") == 0)
		_treat_return(split_line, begin->second, default_route);
	else if (split_line.at(0).compare("index") == 0)
		_treat_index(split_line, begin->second, default_route);
	else if (split_line.at(0).compare("error_page") == 0)
		_treat_error_page(split_line, begin->second, default_route);
	else if (split_line.at(0).compare("location") == 0)
		_treat_location(location_iterator, begin);
	else
		SYNTAX_ERROR(begin->second, split_line.at(0));
}

/* Treat directives in a location context only */
void	Loader::_treatment_location(t_vector_pair_iterator &begin, Route &default_route, t_vector_iterator &location_iterator)
{
	t_vector_string tmp_split;
	if ((begin->first).find("\"") != std::string::npos)
	{
		t_vector_string	tmp_split_quote = split(begin->first, '"');
		tmp_split = split(tmp_split_quote.at(0), ' ');
		tmp_split.push_back(tmp_split_quote.at(1));
	} else 
		tmp_split = split(begin->first, ' ');

	if (tmp_split.at(0).compare("limit_except") == 0)
		_treat_limit_except(tmp_split, begin->second, default_route);
	else if (tmp_split.at(0).compare("cgi") == 0)
		_treat_cgi(tmp_split, begin->second, default_route);
	else if (tmp_split.at(0).compare("upload") == 0)
		_treat_upload(tmp_split, begin->second, default_route);
	else
		_treatment_common(tmp_split, begin, default_route, location_iterator);
}

/* Treat directives in server context only */
void	Loader::_treatment_server(t_vector_pair_iterator &begin, Server &new_server, Route &default_route, t_vector_iterator &location_iterator)
{
	t_vector_string tmp_split;
	if ((begin->first).find("\"") != std::string::npos)
	{
		t_vector_string	tmp_split_quote = split(begin->first, '"');
		tmp_split = split(tmp_split_quote.at(0), ' ');
		tmp_split.push_back(tmp_split_quote.at(1));
	} else 
		tmp_split = split(begin->first, ' ');

	if (tmp_split.at(0).compare("listen") == 0)
		_treat_listen(tmp_split, begin->second, new_server);
	else if (tmp_split.at(0).compare("server_name") == 0)
		_treat_server_name(tmp_split, begin->second,  new_server);
	else
		_treatment_common(tmp_split, begin, default_route, location_iterator);
}

void	Loader::_treat_listen(t_vector_string split_line, unsigned int line, Server &new_server)
{
	if (split_line.size() <= 1 || split_line.size() > 3)
		SYNTAX_ERROR(line, split_line.at(0));
	if (split_line.size() == 2)
	{
		if (!is_number(split_line.at(1)))
			SYNTAX_ERROR(line, split_line.at(0));
		new_server.port = split_line.at(1);
	}
	else
	{
		if (!is_number(split_line.at(2)))
			SYNTAX_ERROR(line, split_line.at(0));
		_check_ip(split_line.at(1), line);
		new_server.address = split_line.at(1);
		new_server.port = split_line.at(2);
	}
}

void	Loader::_treat_server_name(t_vector_string split_line, unsigned int line, Server &new_server)
{
	if (split_line.size() < 2)
		SYNTAX_ERROR(line, split_line.at(0));
	new_server.server_name.clear();
	for (t_vector_string_iterator it = split_line.begin() + 1; it != split_line.end(); it++)
		new_server.server_name.push_back(*it);
}

void	Loader::_treat_root(t_vector_string split_line, unsigned int line, Route &default_route)
{
	if (default_route.is_root_set)
		SYNTAX_ERROR(line, split_line.at(0));
	if (split_line.size() < 2 || split_line.size() > 3)
		SYNTAX_ERROR(line, split_line.at(0));
	if (split_line.size() == 2 && split_line.at(1).compare("alias") == 0)
		SYNTAX_ERROR(line, split_line.at(0));
	char * tmp = realpath(split_line.at(1).c_str(), NULL);
	default_route.root.first = tmp ? tmp : "" ;
	free(tmp);
	if (default_route.root.first.empty())
		SYNTAX_ERROR(line, split_line.at(0));
	if (split_line.size() == 3)
	{
		if (split_line.at(2).compare("alias") == 0)
			default_route.root.second = true;
		else
			SYNTAX_ERROR(line, split_line.at(0));
	}
	default_route.is_root_set = true;
}
void	Loader::_treat_autoindex(t_vector_string split_line, unsigned int line, Route &default_route)
{
	if (split_line.size() != 2)
		SYNTAX_ERROR(line, split_line.at(0));
	if (split_line.at(1).compare("on") != 0 && split_line.at(1).compare("off") != 0)
		SYNTAX_ERROR(line, split_line.at(0));
	default_route.autoindex = split_line.at(1);
}

void	Loader::_treat_max_body_size(t_vector_string split_line, unsigned int line, Route &default_route)
{
	if (split_line.size() != 2)
		SYNTAX_ERROR(line, split_line.at(0));
	if (!is_number(split_line.at(1)))
		SYNTAX_ERROR(line, split_line.at(0));
	default_route.max_body_size = split_line.at(1);
}

void	Loader::_treat_return(t_vector_string split_line, unsigned int line, Route &default_route)
{
	if (split_line.size() != 3)
		SYNTAX_ERROR(line, split_line.at(0));
	if (!is_number(split_line.at(1)) || (to_number<int>(split_line.at(1)) < 300 || to_number<int>(split_line.at(1)) > 399))
		SYNTAX_ERROR(line, split_line.at(0));
	default_route.return_.first = split_line.at(1);
	default_route.return_.second = split_line.at(2);
}

void	Loader::_treat_index(t_vector_string split_line, unsigned int line, Route &default_route)
{
	if (split_line.size() < 2)
		SYNTAX_ERROR(line, split_line.at(0));
	default_route.index.clear();
	for (t_vector_string_iterator tmp_it = split_line.begin() + 1; tmp_it != split_line.end(); ++tmp_it)
		default_route.index.push_back(*tmp_it);
}

void	Loader::_treat_location(t_vector_iterator &location_iterator, t_vector_pair_iterator &it)
{
	location_iterator.push_back(it);
	while (it != _config_tab.end() && (it->first).compare("}") != 0)
	{
		if ((it + 1) != _config_tab.end())
			++it;
		else
			SYNTAX_ERROR(it->second, it->first);
		if ((it->first).find("location") != std::string::npos)
		{
			while (it != _config_tab.end() && (it->first).compare("}") != 0)
				++it;
			if ((it) != _config_tab.end())
				++it;
			else
				SYNTAX_ERROR((--it)->second, (--it)->first);
		}
	}
}

void	Loader::_treat_limit_except(t_vector_string split_line, unsigned int line, Route &default_route)
{
	if (split_line.size() < 2)
		SYNTAX_ERROR(line, split_line.at(0));

	default_route.limit_except.clear();
	for (size_t i = 1; i < split_line.size(); i++)
	{
		if (split_line.at(i) != "GET" && split_line.at(i) != "POST"
			&& split_line.at(i) != "PUT" && split_line.at(i) != "HEAD"
			&& split_line.at(i) != "DELETE")
			SYNTAX_ERROR(line, split_line.at(0));
		default_route.limit_except.push_back(split_line.at(i));
	}
}

void	Loader::_treat_cgi(t_vector_string split_line, unsigned int line, Route &default_route)
{
	if (split_line.size() != 2)
		SYNTAX_ERROR(line, split_line.at(0));
	char * tmp = realpath(split_line.at(1).c_str(), NULL);
	default_route.cgi = tmp ? tmp : "" ;
	free(tmp);
	if (default_route.cgi.empty())
		SYNTAX_ERROR(line, split_line.at(0));
}

void	Loader::_treat_upload(t_vector_string split_line, unsigned int line, Route &default_route)
{
	if (split_line.size() != 2)
		SYNTAX_ERROR(line, split_line.at(0));

	char * tmp = realpath(split_line.at(1).c_str(), NULL);
	default_route.upload = tmp ? tmp : "";
	free(tmp);
	if (default_route.upload.empty())
		SYNTAX_ERROR(line, split_line.at(0));
}

void	Loader::_treat_error_page(t_vector_string split_line, unsigned int line, Route &default_route)
{
	if (split_line.size() < 3)
		SYNTAX_ERROR(line, split_line.at(0));
	if (split_line.at(split_line.size() - 1)[0] != '/')
		SYNTAX_ERROR(line, split_line.at(0));
	for (size_t i = 1; i < split_line.size() - 1; ++i)
	{
		if (!is_number(split_line.at(i)) || (to_number<int>(split_line.at(i)) < 400 || to_number<int>(split_line.at(i)) > 599))
			SYNTAX_ERROR(line, split_line.at(0));
		if ((default_route.error_page.find(split_line.at(i))) != default_route.error_page.end())
			SYNTAX_ERROR(line, split_line.at(0));
		default_route.error_page.insert(std::make_pair(split_line.at(i), split_line.at(split_line.size() - 1)));
	}
}

/* Check the validity of the IP address */
void	Loader::_check_ip(const std::string& ip, unsigned int line)
{
	t_vector_string	tmp_split = split(ip, '.');
	if (tmp_split.size() != 4)
		SYNTAX_ERROR(line, tmp_split.at(0));
	for (t_vector_string_iterator it = tmp_split.begin(); it != tmp_split.end(); ++it)
	{
		if (!is_number(*it))
			SYNTAX_ERROR(line, tmp_split.at(0));
		if (!is_in_range(to_number<int>(*it), 0, 255))
			SYNTAX_ERROR(line, tmp_split.at(0));
	}
}

void	Loader::_get_location_name(t_vector_string  &split_line, unsigned int line, Route &default_route)
{
	if (split_line.size() < 2 || split_line.size() > 3)
		SYNTAX_ERROR(line, split_line.at(0));
	if (split_line.size() == 2 && (split_line.at(1).compare("{") == 0))
		SYNTAX_ERROR(line, split_line.at(0));
	if (split_line.size() == 3 && (split_line.at(2).compare("{") != 0))
		SYNTAX_ERROR(line, split_line.at(0));
	if (split_line.at(1)[0] != '/' && split_line.at(1)[0] != '.')
		SYNTAX_ERROR(line, split_line.at(0));
	if (split_line.at(1).find(".") != std::string::npos)
	{
		split_line = split(split_line.at(1), '.');
		if (split_line.size() == 2)
		{
			default_route.location = split_line.at(0);
			default_route.ext = split_line.at(1);
		}
		else if (split_line.size() == 1)
		{
			default_route.ext = split_line.at(0);
		}
		else
			SYNTAX_ERROR(line, split_line.at(0));
	}
	else
		default_route.location = split_line.at(1);
	default_route.is_root_set = false;
}
