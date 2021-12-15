#include "Request.hpp"

Request::~Request() {
}


Request::Request() : 
	content_length(-1),
	chunked(false),
	delim(CRLF)
{
}

Request::Request(Request const & ref) {
	*this = ref;
}

Request & Request::operator=(const Request & ref) {
	method = ref.method;
	uri = ref.uri;
	host = ref.host;
	query = ref.query;
	content_length = ref.content_length;
	chunked = ref.chunked;
	headers = ref.headers;
	body = ref.body;
	delim = ref.delim;
	return *this;
}

void Request::what() const {
	std::cout << "method : {" <<	method			<< "}\n";	
	std::cout << "uri : {" <<	uri				<< "}\n";	
	std::cout << "host : {" <<	host			<< "}\n";	
	std::cout << "query : {" <<	query			<< "}\n";	
	std::cout << "content_length : {" << 	content_length 	<< "}\n";
	std::cout << "chunked : {" <<	chunked			<< "}\n";	
	std::cout << "body : {" <<	body			<< "}\n";	
	std::cout << "delim : {" <<	delim			<< "}\n";	
	std::cout << "headers : {" << "\n";	
	std::for_each(headers.begin(), headers.end(), _what_map<std::string, std::string>);
	std::cout << "}" << "\n";	
}
