#pragma once
#include <vector>
#include <iostream>
#include <sstream>
#include <map>
#include <string>
#include <algorithm>
#include "utils.hpp"

class Request {

	public:

	std::string							method;
	std::string							uri;
	std::string							host;
	ssize_t								content_length;
	bool								chunked;
	std::map<std::string, std::string>	headers;
	std::string 						body;
	std::string							delim;
	std::string							query;

	Request();
	Request(const Request & ref);
	~Request(void);
	Request & operator=(const Request & ref);

	void what(void) const;
};
