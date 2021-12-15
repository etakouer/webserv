#pragma once

#include <string>
#include <iostream>
#include <map>
#include <vector>

class Route
{
public:
	std::pair<std::string, bool> root;
	std::string autoindex;
	std::string max_body_size;
	std::string location;
	std::vector<std::string> limit_except;
	std::map<std::string, std::string> error_page;
	std::pair<std::string, std::string> return_;
	std::string ext;
	std::vector<std::string> index;
	std::string cgi;
	std::string upload;
	bool		is_root_set;

public:
	Route();
	~Route();
	void what() const;

};
