#include "Worker.hpp"
#include <limits.h>

template<class T>
void gwhat(T & obj) { obj.what(); }

Worker::Worker(const std::map<int, Socket_server> & socket_servers) :
	_socket_servers(socket_servers)
{
	std::map<int, Socket_server>::iterator last = _socket_servers.end();
	std::map<int, Socket_server>::iterator it = _socket_servers.begin();

	/* resize vectors for first kevent loop */
	_event_list.resize(_socket_servers.size() + MAX_EVENT);
	_modif_list.resize(_socket_servers.size());
	/* register listeners */
	for (int i = 0; it != last; it++)
		EV_SET(&(_modif_list[i++]), it->second.fd,
				EVFILT_READ, EV_ADD, 0, 0, NULL);
}

Worker::Worker(const Worker & ref)
{
	*this = ref;
}

Worker::~Worker(void)
{
	;
}

Worker & Worker::operator=(const Worker & right) 
{
	_event_list = right._event_list;
	_modif_list = right._modif_list;
	_socket_clients = right._socket_clients;
	_socket_servers = right._socket_servers;
	_closed_clients = right._closed_clients;
	return *this;
}

void Worker::update_modif_list(int ident, int16_t filter,
		uint16_t flags, uint32_t fflags, intptr_t data, void *udata)
{
	struct kevent event;

	EV_SET(&event, ident, filter, flags, fflags, data, udata);
	_modif_list.push_back(event);
}

void Worker::new_client(int i)
{
	int 				new_client;
	struct sockaddr_in	from;
	socklen_t			slen;
	int					max_back = MAX_BACK;
	
	if (_event_list[i].flags & EV_EOF)
		/* returns the socket error (if any) in fflags */
		throw std::runtime_error(std::string(strerror(_event_list[i].fflags)));
	slen = sizeof(from);
#ifdef DEBUG
	std::cout << "[Worker] -   Backlog : " << _event_list[i].data << "\n";
#endif
	/* data contains the size of the listen backlog. By limiting new client 
	 * to max_back, load can be smooth around multiple kevent cycle */
	while (_event_list[i].data-- && max_back--)
	{
		if ((new_client = accept(_event_list[i].ident,
						(struct sockaddr *)&from, &slen)) == -1)
			throw std::runtime_error(std::string(strerror(errno)));
		/* Setup client's timeout for request line and headers */
		update_modif_list(new_client, EVFILT_TIMER, EV_ADD | EV_ONESHOT,
						NOTE_SECONDS, TO_HEADERS);
		update_modif_list(new_client, EVFILT_READ, EV_ADD);
		/* Store client's addr and port as string */
		char		buffer[INET_ADDRSTRLEN];
		inet_ntop(AF_INET, &(from.sin_addr), buffer, INET_ADDRSTRLEN);
		uint16_t 	port = htons(from.sin_port);
		Socket_client client = Socket_client(new_client, buffer,
						to_string(port), 
						&(_socket_servers.find(_event_list[i].ident)->second));
		_socket_clients.insert(std::make_pair(new_client, client));
#ifdef DEBUG
		std::cout << "[Worker] -   new client -> " ;
		_socket_clients.find(new_client)->second.what();
		std::cout << "\n";
#endif
	}
}

void Worker::recv_client(int i, Socket_client * client_ptr)
{
	char	buffer[_event_list[i].data];
	Socket_client & client = *client_ptr;
	int nb_read;

#ifdef DEBUG
	std::cout << "[Worker] -  recv client -> ";
	client.what();
	std::cout << ", " << _event_list[i].data << " bytes to read";
	std::cout << "\n";
#endif 
	if ((nb_read = read(client.fd, buffer, _event_list[i].data)) == -1)
	{
		del_client(i, &client);
		throw std::runtime_error(std::string(strerror(errno)));
	}
	if (nb_read != 0)
		client.buffer_recv.append(buffer, _event_list[i].data);
	/* After receiving data, we trigger process_client */
	if (!(client.state & RESPONSE) && !(client.state & READY))
		process_client(client);
}


void Worker::send_client(int i, Socket_client * client_ptr)
{
	ssize_t			size_send;
	Socket_client & client = *client_ptr;
#ifdef DEBUG
	std::cout << "[Worker] -  send client -> ";
	client.what();
	std::cout << ", " << client.buffer_send.size() << " bytes to send, ";
	std::cout << _event_list[i].data << " bytes in pipe";
	std::cout << "\n";
#endif
	/* kevent will put the number of bytes availaible in the send buffer
	 * inside the .data attribute, if it is to narrow to send our full
	 * buffer, we must choose the pipe's size and register another send */
	size_send = std::min((size_t)_event_list[i].data, client.buffer_send.size());
	size_send = write(client.fd, client.buffer_send.c_str(), size_send);
	/* if size_send returns -1, we might have received a SIG_PIPE */
	if (size_send == -1)
		return del_client(i, &client);
	update_modif_list(client.fd, EVFILT_TIMER, EV_ADD | EV_ONESHOT,
											NOTE_SECONDS, TO_SEND);
	if (size_send != 0)
		client.buffer_send.erase(0, size_send);
	if (client.buffer_send.empty() && client.response.read_end)
	{
		if (client.closed || client.request.headers["connection"] == "close")
			del_client(i, &client);
		else {
			update_modif_list(client.fd, EVFILT_WRITE, EV_DELETE | EV_CLEAR);
			client.clean();
			/* Setup client's timeout for sending back data */
			update_modif_list(client.fd, EVFILT_TIMER, EV_ADD | EV_ONESHOT,
												NOTE_SECONDS, TO_HEADERS);
			if (!client.buffer_recv.empty()) 
				process_client(client);
		}
	}
}

void Worker::del_client(int i, Socket_client * client_ptr)
{
	Socket_client & client = *client_ptr;
#ifdef DEBUG
	std::cout << "[Worker] -   " << 
	((_event_list[i].filter == EVFILT_TIMER) ? "TO" : "del") 
	<< " client -> ";
	client.what();
	std::cout << "\n";
#endif 
	/* Delete timeout if it was not triggered */
	if (_event_list[i].filter != EVFILT_TIMER)
		update_modif_list(client.fd, EVFILT_TIMER, EV_DELETE);
	/* if client was running a CGI and we did not send at least headers
	 * we must kill his CGI and notify a TIMEOUT via 504 */
	else if (client.action == ACTION_CGI && !client.response.head_send)
	{
		if (client.cgi.pid != -1)
		{
			kill(client.cgi.pid, SIGKILL);
			waitpid(client.cgi.pid, &client.cgi.exit_code, WNOHANG);
			client.action = ACTION_NORMAL;
			client._update_stat(RESPONSE | ERROR, 504);
			process_client(client);
			return ;
		}
	}
	close(client.fd_read);
	close(client.fd_write);
	close(client.fd);
	_closed_clients.insert(client.fd);
	_socket_clients.erase(client.fd);
}

// I/O - load files, CGI pipe output 
void Worker::read_client(int i, Socket_client * client_ptr)
{
	Socket_client & client = *client_ptr;
#ifdef DEBUG
	std::cout << "[Worker] -   Read client -> ";
	client.what();
	std::cout << ", " << _event_list[i].data << " bytes to read\n";
#endif 
	ssize_t nb_read = NB_READ;
	if (client.action == ACTION_CGI)
		nb_read = _event_list[i].data; 	

	char buffer[nb_read];
	ssize_t size_read = read(client.fd_read, buffer, nb_read);

	if (size_read == -1) {
		client._update_stat(RESPONSE | ERROR, 500);
		close(client.fd_read);
		client.fd_read = -1;
		process_client(client);
		return ;	
	}
	if (size_read != 0)
		client.response.body.append(buffer, size_read);
	if (client.action != ACTION_CGI)
	{
		if (size_read == _event_list[i].data)
		{
			close(client.fd_read);
			client.fd_read = -1;
			client.response.read_end = true;
			client.state = READY;
			process_client(client);
			return ;
		}
	}
	else if (_event_list[i].flags & EV_EOF)
	{
		close(client.fd_read);
		client.fd_read = -1;
		waitpid(client.cgi.pid, &client.cgi.exit_code, WNOHANG);	
		client.cgi.pid = -1;
		client.response.read_end = true;
		client.state = READY;
		try {
			process_client(client);
		} catch (std::exception & e) {
			process_client(client);
		}
		return ;
	}
	if (size_read == NB_READ)
	{
		client.state = READY;
		process_client(client);
	}
}

void Worker::abort_write(Socket_client & client) 
{
	close(client.fd_write);
	close(client.fd_read);
	client.fd_read = -1;
	client.fd_write = -1;
	client.action = ACTION_NORMAL;
	client._update_stat(RESPONSE | ERROR, 502);
	process_client(client);
}

// I/O - upload files, CGI pipe input 
void Worker::write_client(int i, Socket_client * client_ptr)
{
	Socket_client & client = *client_ptr;
	ssize_t			size_write = client.request.body.size();
#ifdef DEBUG
	std::cout << "[Worker] -   Write client -> ";
	client.what();
	std::cout << ", " << _event_list[i].data << " bytes to write\n";
#endif 

	if (_event_list[i].flags & EV_ERROR)
	{
		abort_write(client);
		return ;
	}
	if (client.action == ACTION_CGI)
	{
		if (waitpid(client.cgi.pid, &client.cgi.exit_code, WNOHANG) == client.cgi.pid)
		{
			client.cgi.pid = -1;
			abort_write(client);
			return ;
		}
		size_write = std::min(client.request.body.size(), (size_t)_event_list[i].data);
	}
	size_write = write(client.fd_write, client.request.body.c_str(), size_write);
	/* if size_send returns -1, we might have received a SIG_PIPE */
	if (size_write == -1) {
		close(client.fd_write);
		client.fd_write = -1;
		if (client.action != ACTION_CGI) {
			client._update_stat(RESPONSE | ERROR, 500);
			process_client(client);
		}
		return;
	}
	if ((size_t)size_write < client.request.body.size())
	{
		if (size_write != 0)
			client.request.body.erase(0, size_write);
	}
	else 
	{
		client.state &= ~NEED_WRITE;
		client.request.body.clear();
		close(client.fd_write);
		client.fd_write = -1;
		if (client.action != ACTION_CGI) {
			client.state = READY;
			client.response.read_end = true;
			process_client(client);
		}
	}
}

void Worker::process_client(Socket_client & client)
{
	#ifdef DEBUG
		std::cout << "[Worker] -   process client -> ";
		client.what();
		std::cout << "\n";
	#endif 
	if (!(client.state & RESPONSE) && !(client.state & READY)) 
	{
		if (client.state & REQUEST_LINE)
			client.process_request_line();
		if (client.state & HEADERS)
			client.process_header_request();
		if (client.state & ROUTE)
		    client.prepare_response();
		if (client.state & BODY) 
		{
			update_modif_list(client.fd, EVFILT_TIMER,
				EV_ADD | EV_ONESHOT, NOTE_SECONDS, TO_BODY);
			client.process_body_request();
		}
		if (client.state & RESPONSE) {
			update_modif_list(client.fd, EVFILT_TIMER,
					EV_ADD | EV_ONESHOT, NOTE_SECONDS, TO_RESPONSE);
			if (client.closed)
				update_modif_list(client.fd, EVFILT_READ, EV_DELETE);
		}
	}
	if (client.state & RESPONSE)
	{
		client.process_response();
		if (client.state & NEED_READ)
			update_modif_list(client.fd_read, EVFILT_READ , EV_ADD,
					0, 0,(void *)((long)client.fd));
		if (client.state & NEED_WRITE)
			update_modif_list(client.fd_write, EVFILT_WRITE, EV_ADD,
					0, 0,(void *)((long)client.fd));
	}
	if(client.state == READY) {
		client.fetch_response();
		update_modif_list(client.fd, EVFILT_WRITE, EV_ADD);
	}
}

Socket_client * Worker::get_client(int i)
{
	std::map<int, Socket_client>::iterator it;

	/* if a client was closed during this kevent cycle, return NULL
	 * pointer and do not handle his remaining kevents */
	if (_closed_clients.find(_event_list[i].ident)
			!= _closed_clients.end())
	   return NULL;	
	/* if an event triggered with a filled .udata, this event must
	 * be an IO associated with a socket_client, we must test the
	 * udata value and ignore the events if client does not exists
	 * anymore, this may be the case if he disconnects while we were
	 * fetching a resource or client is timeout */
	if (_event_list[i].udata) {
		if ((it = _socket_clients.find((long)_event_list[i].udata))
			== _socket_clients.end())	
			return NULL;
	}
	/* finally, if client does not even exists in our tracking 
	 * structures we ignore the incoming events. */
	else if ((it = _socket_clients.find (_event_list[i].ident))
			== _socket_clients.end())
			return NULL;
	return &(it->second);
}

void Worker::event_loop(void)
{
	int					kq;
	std::map<int, Socket_server>::iterator last = _socket_servers.end();
	Socket_client * client_ptr;

	signal(SIGPIPE, SIG_IGN);
	if ((kq = kqueue()) == -1)
		throw std::runtime_error(std::string(strerror(errno)));
	while (1)
	{
		if ((number_of_events = kevent(kq, _modif_list.begin().base(),
			_modif_list.size(), _event_list.begin().base(),
			_event_list.size(), NULL)) == -1)
			throw std::runtime_error(std::string(strerror(errno)));
		_modif_list.resize(0);
		_closed_clients.clear();
#ifdef DEBUG
		std::cout << "-----\n" ;
#endif
		for (int i = 0; i < number_of_events; i++)
		{
			try 
			{ 
				/* new client has connected */
				if (_socket_servers.find(_event_list[i].ident) != last)
					new_client(i);
				else
				{
					/* does this client need processing ? */
					if (!(client_ptr = get_client(i)))
						continue ;
					else if  (!(_event_list[i].udata) && 
							(_event_list[i].flags & EV_EOF ||
							 _event_list[i].filter == EVFILT_TIMER))
						del_client(i, client_ptr);
					else
					{
						if (_event_list[i].filter == EVFILT_READ)
						{
							/* I/O, CGI's pipe output, load file */
							if (_event_list[i].udata)
								read_client(i, client_ptr);
							else
							/* client reception */
								recv_client(i, client_ptr);
						}
						else if (_event_list[i].filter == EVFILT_WRITE)
						{
							/* I/O, CGI's pipe input, upload file */
							if (_event_list[i].udata)
								write_client(i, client_ptr);
							else
							/* client emission */
								send_client(i, client_ptr);
						}
					}
				}
			}
			catch (std::exception & e)
			{
				std::cerr << "Worker::event_loop: " << e.what() << std::endl;
			}
		}
	}
}
