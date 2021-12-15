#include <iomanip>
#include <sstream>
#include "Socket_client.hpp"

extern std::map<short, std::string> status_msgs;
extern std::map<short, std::string> default_pages;

Socket_client::Socket_client(int fd, const std::string & addr, 
								const std::string & port, Socket_server * socket_server) :
	fd(fd),
	addr(addr),
	port(port),
	state(REQUEST_LINE),
	socket_server(socket_server),
	fd_read(-1),
	fd_write(-1),
	closed(false)
{
	;
}

Socket_client::Socket_client(const Socket_client & ref)
{
	*this = ref;
}


Socket_client::~Socket_client(void)
{
	;
}

Socket_client & Socket_client::operator=(const Socket_client & ref)
{
	fd = ref.fd;
	buffer_recv = ref.buffer_recv;
	buffer_send = ref.buffer_send;
	addr = ref.addr;
	port = ref.port;
	state = ref.state;
	cgi = ref.cgi;
	request = ref.request;
	response = ref.response;
	route = ref.route;
	socket_server = ref.socket_server;
	fd_read = ref.fd_read;
	fd_write = ref.fd_write;
	server = ref.server;
	action = ref.action;
	closed = ref.closed;
	return *this;
}

void Socket_client::clean(void)
{
	buffer_send.clear();
	state = REQUEST_LINE;
	request = Request();
	response = Response();
	cgi = Cgi();
	route = Route();
	fd_read = -1;
	fd_write = -1;
	action = 0;
}

void Socket_client::what_state(void) const
{
	std::cout << "----- Socket_client's state : -----" << std::endl;
	if (state & REQUEST_LINE)
		std::cout << "REQUEST_LINE" << std::endl;
	if (state & HEADERS)
		std::cout << "HEADERS" << std::endl;
	if (state & ROUTE)
		std::cout << "ROUTE" << std::endl;
	if (state & BODY)
		std::cout << "BODY" << std::endl;
	if (state & RESPONSE)
		std::cout << "RESPONSE" << std::endl;
	if (state & NEED_READ)
		std::cout << "NEED_READ" << std::endl;
	if (state & NEED_WRITE)
		std::cout << "NEED_WRITE" << std::endl;
	if (state & READY)
		std::cout << "READY" << std::endl;
	if (state & ERROR)
		std::cout << "ERROR" << std::endl;
	std::cout << "----- end state : -----" << std::endl;
}

void Socket_client::what(void) const
{
	std::cout << "[" << fd << "] - " << addr << ":" << port;
}

void Socket_client::big_what(void) const
{
	std::cout << "fd : {" << fd << "}\n";
	std::cout << "addr : {" << addr << "}\n";
   	std::cout << "port : {" << port << "}\n";
	std::cout << "buffer_recv : {" << buffer_recv << "}\n";
	std::cout << "buffer_send : {" << buffer_send << "}\n";
	what_state();
	request.what();
	response.what();
	route.what();
	cgi.what();
	std::cout << "fd_read : {" << fd_read << "}\n";
	std::cout << "fd_write : {" << fd_write << "}\n";
	socket_server->big_what();
}

void Socket_client::generate_directory_listing(void)
{
	std::string					path = _process_build_path();
	DIR *						dirp = opendir(path.c_str());
	struct dirent * 			dp;
	struct stat					inf;
	char						buf[100];
	static const	std::string	top = "<!DOCTYPE html>\r\n<html>\r\n\t<head>\r\n\t<title>";
	static const	std::string bot = "\t</table>\r\n\t</body>\r\n</html>";
	static const	std::string col = "<tr>\r\n<th scope=\"col\">Name</th>\r\n<th scope=\"col\">"
		"Last-modified</th>\r\n<th scope=\"col\">Size</th>\r\n</tr>\r\n";
	static const	std::string sep = "\t\t<tr><th colspan=\"5\"><hr></th></tr>\r\n";

	if (!dirp)
	{
		throw std::runtime_error(strerror(errno));
	}
	response.body.append(top);
	response.body.append("Index of " + request.uri + "</title>\r\n\t</head>\r\n\t<body>\r\n\t" +
				"<h1>Index of " + request.uri + "</h1>\r\n\t<table>\r\n");
	response.body.append(col);
	response.body.append(sep);
	while ((dp = readdir(dirp)) != NULL)
	{
		std::string file = path + "/" + dp->d_name;
		std::string link = request.uri;

		if (*(link.end() - 1) != '/')
			link += "/";
		link += dp->d_name;
		if (stat(file.c_str(), &inf) == -1)
			throw std::runtime_error(strerror(errno));
		if (!strftime(buf, 100, "%a, %d %b %y %T GMT", gmtime(&inf.st_mtimespec.tv_sec)))
			throw std::runtime_error("strftime");
		response.body.append("\t\t<tr><td valign=\"top\"><a href=\"" + link + "\">" +
					/* Name */
					std::string(dp->d_name) + "</a></td>" +
					/* Last-modified */
					"<td align=\"right\">" + buf + "</td>" +
					/* Size */
					"<td align=\"right\">" + to_string(inf.st_size) + "</td></tr>\r\n");
	}
	response.body.append(sep);
	response.body.append(bot);
	closedir(dirp);
	state = READY;
	response.status = 200;
	response.content_length = response.body.size();
	response.content_type = "text/html";
	response.read_end = true;
}

void Socket_client::_setup_cgi()
{
	std::map<std::string, std::string>::iterator ct =
						request.headers.find("content-type");
	std::map<std::string, std::string>::iterator co =
						request.headers.find("cookie");
	std::string real_path = _process_build_path();

	cgi.content_type = "CONTENT_TYPE=" + (ct == request.headers.end() ?
														"" : ct->second);
	cgi.http_cookie = "HTTP_COOKIE=" + (co == request.headers.end() ?
														"" : co->second);
	cgi.content_length = "CONTENT_LENGTH=" + to_string(request.content_length);

	cgi.path_info = "PATH_INFO=" + request.uri;
	cgi.query_string = "QUERY_STRING=" + request.query;
	cgi.request_method = "REQUEST_METHOD=" + request.method;
	cgi.path_translated = "PATH_TRANSLATED=" + real_path;
	cgi.script_name = "SCRIPT_NAME=" + real_path;
	cgi.script_filename = "SCRIPT_FILENAME=" + real_path;
	cgi.request_uri = "REQUEST_URI=" + request.uri + "?" + request.query;
	cgi.document_uri = "DOCUMENT_URI=" + request.uri;
	cgi.remote_addr = "REMOTE_ADDR=" + addr;
	cgi.remote_port = "REMOTE_PORT=" + port;
	cgi.server_port = "SERVER_PORT=" + server->port;
	cgi.server_name = "SERVER_NAME=" + server->server_name[0];

	cgi.path = "PATH=" + (getenv("PATH") ? std::string(getenv("PATH")) : "");
	cgi.pwd = "PWD=" + (getenv("PWD") ? std::string(getenv("PWD")) : "");

	/* extract X- and HTTP- headers */
	for (std::map<std::string, std::string>::iterator it =
		request.headers.begin(); it != request.headers.end(); it++)
	{
		std::string header = _toupper(it->first);
		if (header.find("HTTP-") == 0 || header.find("X-") == 0)
		{
			cgi.special_headers.push_back(std::string("HTTP_") + header);
			std::string & back = cgi.special_headers.back();
			for (size_t i = 0; i < back.size(); i++)
				if (back[i] == '-')
					back[i] = '_';
			back += ("=" + it->second);
			cgi.envp.push_back(back.begin().base());
		}
	}

	/* static environment */
	cgi.envp.push_back(cgi.server_protocol.begin().base());
	cgi.envp.push_back(cgi.request_scheme.begin().base());
	cgi.envp.push_back(cgi.gateway_interface.begin().base());
	cgi.envp.push_back(cgi.server_software.begin().base());
	/* ------------------ */
	cgi.envp.push_back(cgi.path.begin().base());
	cgi.envp.push_back(cgi.pwd.begin().base());
	cgi.envp.push_back(cgi.query_string.begin().base());
	cgi.envp.push_back(cgi.request_method.begin().base());
	cgi.envp.push_back(cgi.content_type.begin().base());
	cgi.envp.push_back(cgi.content_length.begin().base());
	cgi.envp.push_back(cgi.script_name.begin().base());
	cgi.envp.push_back(cgi.script_filename.begin().base());
	cgi.envp.push_back(cgi.request_uri.begin().base());
	cgi.envp.push_back(cgi.document_uri.begin().base());
	cgi.envp.push_back(cgi.remote_addr.begin().base());
	cgi.envp.push_back(cgi.remote_port.begin().base());
	cgi.envp.push_back(cgi.server_name.begin().base());
	cgi.envp.push_back(cgi.path_info.begin().base());
	cgi.envp.push_back(cgi.http_cookie.begin().base());
	cgi.envp.push_back(NULL);
}

void Socket_client::_prepare_pipes(void)
{

#ifdef DEBUG
	std::cout << "[Cgi] - start CGI initialization" << std::endl;
#endif
	if (pipe(cgi.input) == -1)
		throw std::runtime_error(std::string(strerror(errno)));
	if (pipe(cgi.output) == -1)
	{
		std::runtime_error error =
				std::runtime_error(std::string(strerror(errno)));
		close(cgi.input[0]);
		close(cgi.input[1]);
		throw error;
	}
	fd_read = cgi.output[0];
	fd_write = cgi.input[1];
}

void Socket_client::_exec_cgi(void)
{
	pid_t		pid;

#ifdef DEBUG
	std::cout << "[Cgi] - start CGI execution" << std::endl;
#endif
	_prepare_pipes();
	if ((pid = fork()) == -1)
	{
		cgi.close_pipe_worker_side();
		cgi.close_pipe_cgi_side();
		throw std::runtime_error(std::string(strerror(errno)));
	}
	else if (pid == 0)
	{
		cgi.close_pipe_cgi_side();
		/* change STDIN to worker's pipe input and STDOUT to
		 * worker's pipe output */
		if (dup2(cgi.input[0], STDIN_FILENO)   == -1 ||
			dup2(cgi.output[1], STDOUT_FILENO) == -1) 
		{
			#ifdef DEBUG 
			std::cerr << "[Cgi] - child dup : " << std::string(strerror(errno)) << std::endl;
			#endif 
			_exit(EXIT_FAILURE);
		}
		/* close all fd's inherited from fork(2) */
		_clean_fd_table();
		if (chdir(route.root.first.c_str()) == -1) {
			#ifdef DEBUG 
			std::cerr << "[Cgi] - child exceve : " << std::string(strerror(errno)) << std::endl;
			#endif 
			_exit(EXIT_FAILURE);
		}
		std::string exe = _process_build_path();
		if (execve(route.cgi.c_str(), (char *[]){route.cgi.begin().base(),
			exe.begin().base(), NULL},
			cgi.envp.begin().base()) == -1) 
		{
			#ifdef DEBUG 
			std::cerr << "[Cgi] - child exceve : " << std::string(strerror(errno)) << std::endl;
			#endif 
			_exit(EXIT_FAILURE);
		}
	
	}
	cgi.close_pipe_worker_side();
	cgi.pid = pid;
}

const std::string & Socket_client::check_method() 
{
	static const std::string	methods[] = {
		"GET", "POST", "PUT", "DELETE", "HEAD", "err"};
	int i;

	for (i = 0; i < 5; i++)
		if (!buffer_recv.compare(0, methods[i].size(),
								methods[i]))
			break;
	return (methods[i]);
}

void Socket_client::_update_stat(int _state, short _status)
{
#ifdef LOG
	if ((_state & ROUTE) && (_state & ERROR))
		std::cout << "\""<< buffer_recv.substr(0,buffer_recv.find(request.delim)) 
			<< "\" [" << addr << ":" << port << "]\n"; 
#endif
	static short closed_status[] = {400, 413, 414, 415, 431, 500, 501, 504, 505};
	static const int size = (sizeof(closed_status)/sizeof(short));
	response.status = _status;
	state = _state;
	closed = std::find(closed_status, closed_status + size, response.status)
													!= (closed_status + size);
}

bool Socket_client::is_valid_uri() {
	size_t pos;
	
	std::string tmp = _tolower(request.uri.substr(0, 8));
	if ((tmp.compare(0, 7, "http://") == 0) || (tmp.compare(0, 8, "https://") == 0))
	{
		pos = request.uri.find("/");
		request.uri.erase(0, pos + 2);
		pos = request.uri.find("/");
		request.host = request.uri.substr(0, pos);
		if (request.host.empty() || (request.host.find("..") != std::string::npos))
			return false;
		request.uri.erase(0, request.host.size());
		request.headers["host"] = request.host;
		request.host = "";
	}
	else if (request.uri[0] != '/')
			return false;
	request.query = server->_delete_uri_variable(request.uri);
	server->_delete_duplicate_slash(request.uri);
	server->_remove_simple_dot(request.uri);
	if (!server->_format_double_dot(request.uri))
		return false;
	server->_decode_uri(request.uri);
	if (request.uri.empty())
		request.uri = "/";
	return true;
}

/* request-line   = method 1*SP request-target 1*SP HTTP-version CRLF */
void Socket_client::process_request_line()
{
	static const std::string	numvers = "1.1";
	static const std::string	version = " HTTP/";
	size_t 						found;
	size_t						spaces = 0;

	found = buffer_recv.find("\n");
	if (found == std::string::npos) {
		if (buffer_recv.size() > SIZE_BUFF)
			return _update_stat(ROUTE | ERROR, 414);
		state = REQUEST_LINE;
		return;
	}
	if (!found || buffer_recv[found - 1] != '\r')
		request.delim = "\n";
	if (found > SIZE_BUFF)
		return _update_stat(ROUTE | ERROR, 414);
	// empty line before request line
	if (buffer_recv.compare(0, request.delim.size(), request.delim) == 0) {
		buffer_recv.erase(0, request.delim.size());
		state = REQUEST_LINE;
		return;
	}
	request.method = check_method();
	if (request.method == "err" || buffer_recv[request.method.size()] != ' ')
		return _update_stat(ROUTE | ERROR, 400);
	while (buffer_recv[request.method.size() + spaces] == ' ')
		++spaces;
	/* is this a " HTTP/" request ? */
	found = buffer_recv.find(version, request.method.size() + spaces);
	if (found == std::string::npos)
		return _update_stat(ROUTE | ERROR, 400);
	/* is this a " HTTP/1.1" request ? */
	if (buffer_recv.compare(found, version.size() + numvers.size() +
				request.delim.size(), version + numvers + request.delim))
		return _update_stat(ROUTE | ERROR, 505);
	while (buffer_recv[--found] == ' ')
		;
	/* found start at index 0, we must add 1 to it because we mixed it with size */
	request.uri = buffer_recv.substr(request.method.size() + spaces, 
								found + 1 - (request.method.size() + spaces));
	if (!is_valid_uri()) 
		return _update_stat(ROUTE | ERROR, 400);
#ifdef LOG
	std::cout << "\""<< buffer_recv.substr(0,buffer_recv.find(request.delim)) 
		<< "\" [" << addr << ":" << port << "]\n"; 
#endif
	buffer_recv.erase(0, buffer_recv.find(request.delim) + request.delim.size());
	state = HEADERS;
}

void Socket_client::process_header_request()
{
	std::string							key;
	std::string							val;
	size_t								found;
	size_t								colon;

	for (;;)
	{
		found = buffer_recv.find(request.delim);
		/* ignore empty line */
		if (found == std::string::npos) {
			state = HEADERS;
			return;
		}
		if (found > SIZE_BUFF)
			return _update_stat(ROUTE | ERROR, 431);
		if (!found)
		{
			if (!request.host.empty())
			{
				if (request.method == "DELETE" && 
						(request.content_length > 0 || request.chunked)) {
					_update_stat(RESPONSE | ERROR, 415);
					return;
				}
				buffer_recv.erase(0, found + request.delim.size());
				if (request.content_length == -1)
					request.content_length = 0;
				if (request.headers.find("host") != request.headers.end())
					request.host = request.headers["host"];
				size_t pos = request.host.find(":");
				if (pos != std::string::npos)
					request.host = request.host.substr(0, pos);
				state = ROUTE;
				return;
			}
			else
				return _update_stat(ROUTE | ERROR, 400);
		}
		colon = buffer_recv.find(":");
		/* ignore invalid headers */
		if (colon == std::string::npos || colon > found)
		{
			buffer_recv.erase(0, found + request.delim.size());
			continue ;
		}
		/* extract key:val, lower key and trim val */
		key = buffer_recv.substr(0, colon);
		key = _tolower(key);
		val = buffer_recv.substr(colon + COLON,
				found - (colon + COLON));
		val = _ltrim(_rtrim(val));
		if (key == "host")
		{
			/* Host duplicate */
			if (!request.host.empty())
				return _update_stat(ROUTE | ERROR, 400);
			request.host = val;
		}
		else if (key == "content-length")
		{
			/* Content-length duplicate */
			if (request.content_length != -1) 
				return _update_stat(ROUTE | ERROR, 400);
			try 
			{
				request.content_length = _extract_content_length(val); 
				if (request.content_length < 0)
					throw std::invalid_argument("invalid content length");
			}
			catch (std::exception & e)
			{ return _update_stat(ROUTE | ERROR, 400); }
		}
		else if (key == "transfer-encoding")
		{
			/* Transfer-encoding duplicate */
			if (request.chunked)
				return _update_stat(ROUTE | ERROR, 400);
			if (_tolower(val) == "chunked")
				request.chunked = true;
			else 
				return _update_stat(ROUTE | ERROR, 501);
		}
		else
			request.headers[key] = val;
		buffer_recv.erase(0, found + request.delim.size());
	}
}


bool Socket_client::get_simple_body() {
	size_t size = request.content_length - request.body.size();
	request.body.append(buffer_recv.substr(0, size));
	size = std::min(buffer_recv.size(), size);
	buffer_recv.erase(0, size);
	return (request.body.size() == (size_t)(request.content_length));
}

bool Socket_client::get_ckunked_body() {
	size_t length = 0;
	size_t pos_delim = 0;
	size_t cursor = 0;
	ssize_t size_chunck = -1;
	bool end = false;
	while (!end) {
		if ((pos_delim = buffer_recv.find(request.delim, cursor)) == std::string::npos) 
			break;
		size_chunck = _hexstr_to_ssize(buffer_recv.substr(cursor, pos_delim));
		if (size_chunck < 0)
			throw std::logic_error("error chunked : hexa size");
		if (pos_delim + request.delim.size() + size_chunck + request.delim.size() > buffer_recv.size())
			break;
		cursor = pos_delim + request.delim.size();
		request.body.append(buffer_recv.substr(cursor, size_chunck));
		cursor += size_chunck;
		if (buffer_recv.compare(cursor, request.delim.size(), request.delim) != 0)
			throw std::logic_error("error chunked : delimiter expected");
		cursor += request.delim.size();
		length += size_chunck;
		if (size_chunck == 0 && buffer_recv.compare(cursor, request.delim.size(), request.delim))
			end = true;
	}
	buffer_recv.erase(0, cursor);
	request.content_length += length;
	return (end);
}


void Socket_client::process_body_request() {
	if (!request.chunked) {
		if (get_simple_body()) {
			if (!(state & ERROR) && request.content_length > _stol(route.max_body_size)) {
				_update_stat(RESPONSE | ERROR, 413);
				return;
			}	
			state = RESPONSE;
			if (request.method == "GET" || request.method == "HEAD")
				request.body.clear();
			return;
		}
	} else {
		try {
			if (get_ckunked_body()) {
				if (!(state & ERROR) && request.content_length > _stol(route.max_body_size)) {
								_update_stat(RESPONSE | ERROR, 413);
								return;
				}	
				if (request.method == "GET" || request.method == "HEAD")
					request.body.clear();
				state =  RESPONSE;
				return ;
			}
		} catch(std::logic_error const & e) {
			#ifdef DEBUG
			std::cout << "body :  {" << request.body << "}\n";
			std::cout << "content-length :  {" << request.content_length << "}\n";
			std::cout << "rest : {" << buffer_recv << "}\n" ;
			std::cout << e.what() << "\n";
			#endif
			if (request.method == "GET" || request.method == "HEAD") {
				request.body.clear();
				state =  RESPONSE;
			}
			else
				_update_stat(RESPONSE | ERROR, 400);
			return ;
		}
	}
	state = BODY;
}


void Socket_client::prepare_response() {
	std::vector<Server *>::iterator it = socket_server->servers.begin();
	while (it != socket_server->servers.end()) {
		if (find((*it)->server_name.begin(), (*it)->server_name.end(), request.host) != (*it)->server_name.end()) {
			route = (*it)->choose_route(request.uri);
			server = *it;
			break;
		}
		it++;
	}
	if (it == socket_server->servers.end()) {
		server = socket_server->servers[0]; 
		route = server->choose_route(request.uri);
	}

	
	if (closed) {
		state = RESPONSE;
		state |= ERROR;
		return ;
	}
	state = BODY;
}

void Socket_client::_set_error(short code)
{
	if (state & ERROR) {
		response.body = default_pages[code];
		state = READY;
		response.read_end = true;
		response.content_length = response.body.size();
		if (request.method == "HEAD")
			response.body.clear();
		response.content_type = "text/html";
		response.status = code;
		action = ACTION_NORMAL;
	}
	else {
		_update_stat(RESPONSE | ERROR, code);
		process_response();
	}
}

void Socket_client::_set_action() {
	if (!route.return_.first.empty())
		action = ACTION_RETURN;
	else if (!route.upload.empty() &&
		(request.method.compare("POST") == 0 || request.method.compare("PUT") == 0))
		action = ACTION_UPLOAD;
	else if (!route.cgi.empty())
		action = ACTION_CGI;
	else
		action = ACTION_NORMAL;
}

void Socket_client::process_response() {	
	if (state & ERROR)
	{
		if (route.error_page.find(to_string(response.status)) != route.error_page.end())
		{
			std::string tmp_host = request.host;
			std::map<std::string, std::string> tmp_headers = request.headers;
			request = Request();
			request.uri = route.error_page[to_string(response.status)];
			route = server->choose_route(request.uri);
			request.method = "GET";
			request.host = tmp_host;
			request.headers = tmp_headers;
		}
		else 
		{
			_set_error(response.status);
			return ;
		}
	}
	if (route.limit_except.size() && find(route.limit_except.begin(),
		route.limit_except.end(), request.method) == route.limit_except.end())
		return _set_error(405);
	_set_action();
	if (action == ACTION_RETURN)
		_process_return();
	else if (action == ACTION_UPLOAD)
		_process_upload();
	else if (action == ACTION_CGI)
		_process_cgi();
	else
		_process_normal();
}

void Socket_client::process_body_response()
{
	std::string size_str;
	size_t	size_chunked;

	if (response.chunked) {
		while(!response.body.empty()) {
			if (!response.head_send)
				size_chunked = std::min(response.body.size(), (size_t)SIZE_CH - buffer_send.size());
			else	
				size_chunked = std::min(response.body.size(), (size_t)SIZE_CH);
			buffer_send.append(_size_to_hexstr(size_chunked) + CRLF);
			buffer_send.append(response.body.substr(0, size_chunked));
			buffer_send.append(CRLF);
			response.body.erase(0, size_chunked);
			if (response.body.size() < SIZE_CH && !response.read_end)
				break;
		}
		if (response.body.empty())
			buffer_send.append(std::string("0") + CRLF + CRLF);
	}
	else {
		buffer_send.append(response.body);
		response.body.clear();
	}
	response.head_send = true;
}

void Socket_client::process_header_generic()
{
	char			buf[100];
	struct timeval	now;

	/* status line */
	buffer_send.append("HTTP/1.1 " + to_string(response.status) +
			+ " " + status_msgs[response.status] + CRLF);
	/* common headers */
	buffer_send.append(std::string("Server: ") +
			WEBSERV_V + CRLF);
	if (gettimeofday(&now, NULL) == -1)
		throw std::runtime_error(strerror(errno));
	if (!strftime(buf, 100, "%a, %d %b %y %T GMT", gmtime(&now.tv_sec)))
		throw std::runtime_error("strftime");
	buffer_send.append(std::string("Date: ") + buf + CRLF);
	buffer_send.append(std::string("Content-Type: ") +
			response.content_type + CRLF);
	if (response.chunked)
		buffer_send.append(std::string("Transfer-Encoding:") +
				"chunked" + CRLF);
	else 
		buffer_send.append(std::string("Content-Length: ") +
				to_string(response.content_length) + CRLF);
	if (action == ACTION_RETURN)
		buffer_send.append(std::string("Location: ") +
				response.location + CRLF);
	buffer_send.append(std::string("Connection: ") +
			((closed || request.headers["connection"] == "close" ) ? 
			 "closed" : "keep-alive") + CRLF);
	/* custom headers */
	for (std::map<std::string, std::string>::iterator it =
		response.headers.begin(); it != response.headers.end(); it++)
		buffer_send.append(it->first + ": " + it->second + CRLF);
	buffer_send.append(CRLF);
}

static void _extract_cgi_headers(std::map<std::string, std::string>
		& cgi_headers, std::string & delim, Response & response, size_t found)
{
	size_t		colon;
	size_t		line = 0;
	size_t		cursor = 0;
	std::string key;
	std::string val;

	while (line != found)
	{
		line = response.body.find(delim, cursor);
		colon = response.body.find(":", cursor);
		if (colon == std::string::npos || colon > found)
			break ;
		/* invalid header */
		if (colon != std::string::npos && colon > line)
		{
			cursor = line + delim.size();
			continue ;
		}
		key = response.body.substr(cursor, colon - cursor);
		key = _tolower(key);
		val = response.body.substr(colon + COLON, line -
				(colon + COLON));
		cgi_headers[key] = val;
		cursor = line + delim.size();
	} 
}

void Socket_client::_populate_headers_CGI(std::map<std::string, std::string> & cgi_headers)
{
	/* search for Status-line, add it if necessary */
	if (cgi_headers.find("status") != cgi_headers.end())
	{
		buffer_send.append(std::string("HTTP/1.1 ") + cgi_headers["status"] + CRLF);
		cgi_headers.erase("status");
	}
	else 
		buffer_send.append("HTTP/1.1 200 OK" + std::string(CRLF));
	/* search for Content-Type/Content-type, add it if necessary */
	if (cgi_headers.find("content-type") != cgi_headers.end())
	{
		buffer_send.append("Content-Type: " + cgi_headers["content-type"] + CRLF);
		cgi_headers.erase("content-type");
	}
	else 
		buffer_send.append("Content-Type: "+ response.content_type + CRLF);
	/* search for Content-length, disable chunked if found */
	if (cgi_headers.find("content-length") != cgi_headers.end())
	{
		buffer_send.append("Content-Length: " + cgi_headers["content-length"] + CRLF);
		cgi_headers.erase("content-length");
		response.chunked = false;
	}
	else 
	{
		buffer_send.append(std::string("Transfer-Encoding: ") + "chunked" + CRLF);
		response.chunked = true;
	}
}

void Socket_client::process_header_CGI()
{
	size_t			found = response.body.find("\n");
	std::string		delim = CRLF;
	char			buf[100];
	struct timeval	now;
	std::map<std::string, std::string> cgi_headers;

	if (found == std::string::npos)
	{
		/* error_page does not apply to CGI response so 
		 * we mustn't go throw process_response */
		action = ACTION_NORMAL;
		_update_stat(RESPONSE | ERROR, 502);
		response.body.clear();
		throw std::exception();
	}
	if (!found || response.body[found - 1] != '\r')
		delim = "\n";
	found = response.body.find(delim + delim);
	if (found == std::string::npos)
	{
		/* error_page does not apply to CGI response so 
		 * we mustn't go throw process_response */
		action = ACTION_NORMAL;
		_update_stat(RESPONSE | ERROR, 502);
		response.body.clear();
		throw std::exception();
	}
	_extract_cgi_headers(cgi_headers, delim, response, found);
	/* A valid CGI response contains at least one header */
	if (cgi_headers.size() < 1)
	{
		/* error_page does not apply to CGI response so 
		 * we mustn't go throw process_response */
		action = ACTION_NORMAL;
		_update_stat(RESPONSE | ERROR, 502);
		response.body.clear();
		throw std::exception();
	}
	/* make a valid HTTP response from CGI output */
	_populate_headers_CGI(cgi_headers);
	/* add custom headers set by CGI script */
	for (std::map<std::string, std::string>::iterator it = cgi_headers.begin();
		it != cgi_headers.end(); it++)
		buffer_send.append(it->first + ": " + it->second + CRLF);
	buffer_send.append(std::string("Server: ") + WEBSERV_V + CRLF);
	if (gettimeofday(&now, NULL) == -1) {
		action = ACTION_NORMAL;
		_update_stat(RESPONSE | ERROR, 500);
		throw std::exception();
	}
	if (!strftime(buf, 100, "%a, %d %b %y %T GMT", gmtime(&now.tv_sec))) {
		action = ACTION_NORMAL;
		_update_stat(RESPONSE | ERROR, 500);
		throw std::exception();
	}
	buffer_send.append(std::string("Date: ") + buf + CRLF);
	buffer_send.append(std::string("Connection: ") + 
			((closed || request.headers["connection"] == "close" ) ? 
			 "closed" : "keep-alive") + CRLF + CRLF);
	response.body.erase(0, found + delim.size() * 2);
}

void Socket_client::process_header_response()
{
	if (action != ACTION_CGI)
		process_header_generic();
	else 
		process_header_CGI();
#ifdef LOG
	std::cout << "\""<< buffer_send.substr(0,buffer_send.find(CRLF)) 
		<< "\" [" << addr << ":" << port << "]\n"; 
#endif
}

void Socket_client::fetch_response()
{
	if (!response.head_send) 
		process_header_response();
	process_body_response();
}

void Socket_client::_process_cgi() {
	try {
		_setup_cgi();
		_exec_cgi(); 
	}
	catch (std::exception & e) 
	{
		return _set_error(500);
	}
	state |= NEED_WRITE;
	state |= NEED_READ;
}

/* If return directive is set
 * Fill the response with the correct values for the return
 * and set READY */
void Socket_client::_process_return()
{
	response.status = to_number<short>(route.return_.first);
	response.location = route.return_.second;
	response.body = default_pages[response.status];
	response.content_length = response.body.size();
	response.content_type = "text/html";
	response.read_end = true;
	state = READY;
	response.read_end = true;
}

void Socket_client::_process_upload()
{
	std::string file = route.upload + "/";
	std::string tmp_filename = request.uri.substr(route.location.size(), request.uri.size());
	file += tmp_filename;

	if (_is_dir(file.c_str()))
		return _set_error(409);
	if (!_is_dir(route.upload.c_str())) 
		return _set_error(500);
	/* File already exist : rewrite the file => 204 */
	if ((fd_write = open(file.c_str(), O_WRONLY | O_TRUNC |
										O_NONBLOCK)) != -1)
	{
		state = NEED_WRITE;
		/* 204 No content */
		response.status = 204; 
		return ;
	}
	else if (errno == EACCES)
		return _set_error(500);
	else if (errno == ENOENT && ((fd_write = open(file.c_str(),
			O_WRONLY | O_CREAT | O_NONBLOCK, 0600)) != -1))
	{
		state = NEED_WRITE;
		/* 201 Created */
		response.status = 201; 
		return ;
	}
	if (errno == ENOENT)
		return _set_error(500);
	_set_error(403);
}

/* Delete everything inside a folder but not the folder */
void Socket_client::_delete_recursively(DIR *dir, std::string& path)
{
	struct dirent *s_dirent;

	if (!dir)
		return _set_error(403);
	while ((s_dirent = readdir(dir)))
	{
		if ((strncmp(s_dirent->d_name, ".", 2) == 0) || (strncmp(s_dirent->d_name, "..", 3) == 0))
			continue ;
		if (s_dirent->d_type == DT_DIR)
		{
			std::string new_path = path + "/" + s_dirent->d_name;
			_delete_recursively(opendir((path + "/" + s_dirent->d_name).c_str()), new_path);
			rmdir((path + "/" + s_dirent->d_name).c_str());
		}
		remove((path + "/" + s_dirent->d_name).c_str());
	}
	closedir(dir);
}

/* Delete method */
void Socket_client::_process_delete(std::string& path)
{
	if (_is_dir(path.c_str()) && *(request.uri.end() - 1) != '/')
		return _set_error(409);
	if (_is_dir(path.c_str()) && (path == route.root.first + "/"))
	{
		/* If the uri is the root path
		 * Delete everything inside directory (not the directory)
		 * and return 403 */
		_delete_recursively(opendir(path.c_str()), path);
		return _set_error(403);
	}
	/* If not a directory (file) delete the file
	 * If a directory, delete everything recursively (nftw) */
	if (!_is_dir(path.c_str()))
	{
		if (unlink(path.c_str()) == 0)
		{
			/* DELETE  OK but no more information to send */
			response.status = 204; 
			state = READY;
			response.read_end = true;
			return ;
		}
	}
	else if (nftw(path.c_str(), _remove, 20, FTW_DEPTH | FTW_PHYS) == 0)
	{
		/* DELETE  OK but no more information to send */
		response.status = 204; 
		state = READY;
		response.read_end = true;
		return ;
	}
	return _set_error(404);
}

/* If index directive is set, test all index 
 * if one exist and can open it => 200 */
int Socket_client::_test_all_index(std::string& path)
{
	Route tmp_route;
	int	fd;
	for (std::vector<std::string>::iterator it = route.index.begin(); it != route.index.end(); ++it)
	{
		if (*(path.end() - 1) != '/' && (*it).at(0) != '/')
			path.push_back('/');
		std::string tmp_path = path + *it;
		fd = open(tmp_path.c_str(), O_RDONLY | O_NONBLOCK);
		if (fd != -1)
		{
			tmp_route = server->choose_route(request.uri + *it);
			if (tmp_route.ext != route.ext) 
			{
				route = tmp_route;
				request.uri += *it;
				process_response();
				close(fd);
				return 1;
			}
			close(fd);
  			if(_open_file_fill_response(tmp_path))
  				return 1;
		}
	}
	return 0;
}

/* Try opening a file and fill the object response with the 
 * necessary values*/
int Socket_client::_open_file_fill_response(std::string& path)
{
	struct stat now;
	char		buf[100];
	Route		tmp_route;

	if ((fd_read = open(path.c_str(), O_RDONLY | O_NONBLOCK)) != -1)
	{
		if (!(state & ERROR))
			response.status = 200;
		response.content_length = _get_file_size(fd_read);
		response.content_type = _get_file_mime(path);
		if (request.method.compare("GET") == 0 && response.content_length > 0)
			state |= NEED_READ;
		else {
			state = READY;
			response.read_end = true;
		}
		if (!fstat(fd_read, &now))
		{
			if (strftime(buf, 100, "%a, %d %b %y %T GMT",
						gmtime(&now.st_mtimespec.tv_sec)))
			{
				long				lm = now.st_mtimespec.tv_sec;
				std::stringstream	ss;
				std::string			last_modified;
				std::string			content_length;

				ss << std::hex << lm;
				last_modified = ss.str();
				/* empty the string stream */
				ss.str(std::string());
				ss << std::hex << response.content_length;
				content_length = ss.str();
				response.headers["Last-Modified"] = buf;
				response.headers["ETag"] = "\"" + last_modified + 
								"T-" + content_length + "O\"";
			}
		}
		if (!(state & NEED_READ))
			close(fd_read);
		return 1;
	}	
	return 0;
}

/* If method is get or head */
void Socket_client::_process_get_head(std::string& path)
{
	/* If is a directory, test all index if they are given
	 * if no index if autoindex on generate directory listing
	 * else error
	 * If not a directory (a file) try opening the file and check errors*/
	if (_is_dir(path.c_str()))
	{
		if (!route.index.empty())
			if (_test_all_index(path))
				return ;
		if (errno == EACCES)
			return _set_error(500);
		if (route.autoindex.compare("on") == 0) {
			try { generate_directory_listing(); }
			catch (std::exception & e) { _set_error(500); }
			return ;
		}
		if (!route.index.empty())
			return _set_error(404);
		return _set_error(403);
	}
	else
	{
		if (_open_file_fill_response(path))
			return ;
		if (errno == EACCES)
			return _set_error(500);
		return _set_error(404);
	}
}

/* Create the path from root and alias */
std::string Socket_client::_process_build_path()
{
	std::string path = request.uri;

	// uri without / but location with /
	if (route.location.size() > request.uri.size())
			path.push_back('/');
	if (!route.root.first.empty())
	{
		if (route.root.second)
			path = path.substr(route.location.size(), request.uri.size());		
		path.insert(0, route.root.first + "/");
	}
	if (action != ACTION_CGI && _is_dir(path.c_str()) && *(path.end() - 1) != '/')
			path.push_back('/');
	server->_delete_duplicate_slash(path);
	return path;
}

/* If no upload or cgi */
void Socket_client::_process_normal()
{
	std::string path = _process_build_path();
	
	if (request.method.compare("GET") == 0 || request.method.compare("HEAD") == 0)
		return _process_get_head(path);
	else if (request.method.compare("DELETE") == 0)
		return	_process_delete(path);	
	else
	{
		/* PUT or POST must have go through CGI or UPLOAD so just return 200 here */
		response.status = 200;
		response.read_end = true;
		state = READY;
	}
}

size_t Socket_client::_get_file_size(int fd)
{
	off_t	size;
	size = lseek(fd, 0, SEEK_END);
	lseek(fd, 0, SEEK_SET);
	return size;
}

bool Socket_client::_is_dir(const char* path)
{
	struct stat buf;
	stat(path, &buf);
	return S_ISDIR(buf.st_mode);
}	
