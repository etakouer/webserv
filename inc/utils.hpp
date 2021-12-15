#pragma once
#include <ftw.h>
#include <vector>
#include <map>
#include <string>
#include <sstream>  
#include <iostream>
#include <exception>
#include <unistd.h>
#include <stdlib.h>

#define SPACE 1
#define COLON 1
#define CRLF "\r\n"
#define WEBSERV_V "webserv/1.20.1"

/* Kevent */
#define MAX_BACK 32
#define MAX_EVENT 16

/* Size buffers */
#define SIZE_CH 0x1FF8
#define SIZE_BUFF 0x2000
#define NB_READ 0x20000

/* Timeouts */
#define TO_HEADERS 480
#define TO_BODY 480
#define TO_RESPONSE 480
#define TO_SEND 480



bool						is_number(const std::string& s);
std::vector<std::string>	split(const std::string& to_split, char delim);
void						syntax_error(unsigned int line, const std::string &c);
int _remove(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf);

template<class T>
T	HexToInt(const std::string &str)
{
	if(str.size()==0)
		return 0;
	std::istringstream iss(str);
	T	result = 0;
	iss >> std::hex >> result;
	return result;
}

template < class T >
T	to_number(std::string& s)
{
	std::stringstream	ss(s);
	T					num;
	ss >> num;
	return num;
}

template < class T >
std::string	to_string(T &n)
{
	std::stringstream ss;
	ss << n;
	return ss.str();
}

template < class T >
bool	is_in_range(T nb, T r1, T r2)
{
	return ((nb >= r1 && nb <= r2) ? true : false);
}

template<class T>
void _what_vector(T elmt) {
	elmt.what();
}

template<class T1, class T2>
void _what_map(std::pair<T1, T2> elmt) {
	std::cout << "{" << elmt.first << ":" << elmt.second << "}\n";
}

typedef enum http_state {
	REQUEST_LINE = 1,
	HEADERS = 2,
	ROUTE = 4,
	BODY = 8,
	RESPONSE = 16,
	NEED_READ = 32,
	NEED_WRITE = 64,
	ERROR = 128,
	READY = 512,
} e_http_state;

typedef enum http_action {
	ACTION_CGI = 1,
	ACTION_UPLOAD = 2,
	ACTION_NORMAL = 4,
	ACTION_RETURN = 8,
} e_http_action;


long		_extract_content_length(const std::string & str);
std::string _ltrim(const std::string &s);
std::string _rtrim(const std::string &s);
std::string _toupper(const std::string & str);
std::string _tolower(const std::string & str);
ssize_t _hexstr_to_ssize(std::string const & hexstr);
std::string _size_to_hexstr(size_t size);
long		_stol(const std::string & str);

void add_status_msgs();
void add_default_pages();
void add_mime_types();
void _clean_fd_table(void);
std::string _get_file_mime(std::string const & tmp_path);
