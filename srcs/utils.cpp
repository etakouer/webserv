#include "utils.hpp"

extern std::map<std::string, std::string> mime_types;

int _remove(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf)
{
	(void)sb;
	(void)typeflag;
	(void)ftwbuf;
	return remove(fpath);
}

long _extract_content_length(const std::string & str)
{
	long long 	result = 0;

	for (std::string::const_iterator it = str.begin();
		it != str.end(); it++)
	{
		if (!isdigit(*it))
			throw std::invalid_argument("stol: invalid number");
		result = result * 10 + (*it - '0');
		if (result > LONG_MAX)
			throw std::out_of_range("stol: out of range number");
	}
	return result;
}

long _stol(const std::string & str)
{
	bool		sign = false;
	long long 	result = 0;

	for (std::string::const_iterator it = str.begin();
		it != str.end(); it++)
	{
		if (!isdigit(*it))
		{
			if (*it == '-')
			{
				if (it != str.begin() || str.size() == 1)
					throw std::invalid_argument("stol: invalid number");
				else
					sign = true;
			}
			else if (*it == '+')
			{
				if (it != str.begin() || str.size() == 1)
					throw std::invalid_argument("stol: invalid number");
			}
			else if (*it == '.' && it != str.begin())
				return (sign ? -1 * result : result);
			else
				throw std::invalid_argument("stol: invalid number");
		}
		else
			result = result * 10 + (*it - '0');
		if (result > LONG_MAX || result < LONG_MIN)
			throw std::out_of_range("stol: out of range number");
	}
	return (sign ? -1 * result : result);
}

bool	is_number(const std::string& s)
{
	for (std::string::const_iterator it = s.begin(); it != s.end(); ++it)
		if (!std::isdigit(*it))
			return false;
	return true;
}

std::vector<std::string> split(const std::string & tosplit, char delim)
{
	std::string buf;
	std::stringstream ss(tosplit);

	std::vector<std::string> tokens;

	while (getline(ss, buf, delim))
		if (!buf.empty())
			tokens.push_back(buf);

	return tokens;

}

void	syntax_error(unsigned int line, const std::string &c)
{
	std::string str = "Syntax error near '" + c + "' at line " + to_string(line); 
	throw std::runtime_error(str);
}

std::string _ltrim(const std::string &s)
{
    size_t start = s.find_first_not_of(" \r\t");
    return (start == std::string::npos) ? "" : s.substr(start);
}

std::string _rtrim(const std::string &s)
{
    size_t end = s.find_last_not_of(" \r\t");
    return (end == std::string::npos) ? "" : s.substr(0, end + 1);
}

std::string _toupper(const std::string & str)
{
	std::string newstr(str);
	for (size_t i = 0; i < str.size(); i++)
		if (str[i] >= 'a' && str[i] <= 'z')
			newstr[i] = str[i] - 32;
	return newstr;
}

std::string _tolower(const std::string & str)
{
	std::string newstr(str);
	for (size_t i = 0; i < str.size(); i++)
		if (str[i] >= 'A' && str[i] <= 'Z')
			newstr[i] = str[i] + 32;
	return newstr;
}

ssize_t _hexstr_to_ssize(std::string const & hexstr) {
	size_t s;   
	if (!isxdigit(hexstr[0]))
		return -1;
	std::stringstream ss;
	ss << std::hex << hexstr;
	ss >> s;
	return s;
}

std::string _size_to_hexstr(size_t size) {
	std::string s;
	std::stringstream ss;
	ss << std::hex << size;
	ss >> s;
	return s;
}

std::string _get_file_mime(std::string const & tmp_path) {
	std::string ext;
	std::string tmp;
	try {
	size_t pos = tmp_path.rfind("/");
		if (pos != std::string::npos) {
			tmp = tmp_path.substr(pos, tmp_path.size() - pos);
			pos = tmp.rfind(".");
			if (pos != std::string::npos)
				ext = tmp.substr( pos + 1, tmp.size());
		}
	}catch (std::exception & e) {

	}
	ext = mime_types[ext];
	if (ext.empty())
		ext = "text/plain";
	return ext; 
}

void _clean_fd_table(void)
{
	size_t	maxfd = 256;
	struct rlimit	rlim;

	if (getrlimit(RLIMIT_NOFILE, &rlim))
		maxfd = rlim.rlim_cur;
	for (size_t i = 3; i < maxfd; i++)
		close(i);
}
