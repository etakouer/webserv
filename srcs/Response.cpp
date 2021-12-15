#include "Response.hpp"

Response::~Response() {
}

Response::Response() : 
	status(0),
	headers(),
	body(""),
	content_length(0),
	content_type("text/plain"),
	chunked(false),
	location(""),
	head_send(false),
	read_end(false)
{
}

Response::Response(Response const & ref) {
	*this = ref;
}

Response & Response::operator=(const Response & ref) {
	status = ref.status;
	headers = ref.headers;
	body = ref.body;
	content_length = ref.content_length;
	content_type = ref.content_type;
	chunked = ref.chunked;
	head_send = ref.head_send;
	read_end = ref.read_end;
	location = ref.location;
	return *this;
}

void Response::what() const {
	std::cout << "status : {" << 	status 	<< "}\n";
	std::cout << "content_length : {" << 	content_length 	<< "}\n";
	std::cout << "content_type : {" << 	content_type 	<< "}\n";
	std::cout << "chunked : {" <<	chunked			<< "}\n";	
	std::cout << "body : {" <<	body			<< "}\n";	
	std::cout << "headers : {" << "\n";	
	std::for_each(headers.begin(), headers.end(), _what_map<std::string, std::string>);
	std::cout << "}" << "\n";	
}
