#include "utils.hpp"

std::map<short, std::string> status_msgs;
std::map<short, std::string> default_pages;
std::map<std::string, std::string> mime_types;

void add_status_msgs() {
	status_msgs[100] = "Continue";
	status_msgs[101] = "Switching Protocols";
	status_msgs[102] = "Processing";
	status_msgs[103] = "Early Hints";
	status_msgs[200] = "OK";
	status_msgs[201] = "Created";
	status_msgs[202] = "Accepted";
	status_msgs[203] = "Non-Authoritative Information";
	status_msgs[204] = "No Content";
	status_msgs[205] = "Reset Content";
	status_msgs[206] = "Partial Content";
	status_msgs[207] = "Multi-Status";
	status_msgs[208] = "Already Reported";
	status_msgs[226] = "IM Used";
	status_msgs[300] = "Multiple Choices";
	status_msgs[301] = "Moved Permanently";
	status_msgs[302] = "Found";
	status_msgs[303] = "See Other";
	status_msgs[304] = "Not Modified";
	status_msgs[305] = "Use Proxy";
	status_msgs[307] = "Temporary Redirect";
	status_msgs[308] = "Permanent Redirect";
	status_msgs[400] = "Bad Request";
	status_msgs[401] = "Unauthorized";
	status_msgs[402] = "Payment Required";
	status_msgs[403] = "Forbidden";
	status_msgs[404] = "Not Found";
	status_msgs[405] = "Method Not Allowed";
	status_msgs[406] = "Not Acceptable";
	status_msgs[407] = "Proxy Authentication Required";
	status_msgs[408] = "Request Timeout";
	status_msgs[409] = "Conflict";
	status_msgs[410] = "Gone";
	status_msgs[411] = "Length Required";
	status_msgs[412] = "Precondition Failed";
	status_msgs[413] = "Content Too Large";
	status_msgs[414] = "URI Too Long";
	status_msgs[415] = "Unsupported Media Type";
	status_msgs[416] = "Range Not Satisfiable";
	status_msgs[417] = "Expectation Failed";
	status_msgs[421] = "Misdirected Request";
	status_msgs[422] = "Unprocessable Content";
	status_msgs[423] = "Locked";
	status_msgs[424] = "Failed Dependency";
	status_msgs[425] = "Too Early";
	status_msgs[426] = "Upgrade Required";
	status_msgs[427] = "Unassigned";
	status_msgs[428] = "Precondition Required";
	status_msgs[429] = "Too Many Requests";
	status_msgs[430] = "Unassigned";
	status_msgs[431] = "Request Header Fields Too Large";
	status_msgs[451] = "Unavailable For Legal Reasons";
	status_msgs[500] = "Internal Server Error";
	status_msgs[501] = "Not Implemented";
	status_msgs[502] = "Bad Gateway";
	status_msgs[503] = "Service Unavailable";
	status_msgs[504] = "Gateway Timeout";
	status_msgs[505] = "HTTP Version Not Supported";
	status_msgs[506] = "Variant Also Negotiates";
	status_msgs[507] = "Insufficient Storage";
	status_msgs[508] = "Loop Detected";
	status_msgs[509] = "Unassigned";
	status_msgs[510] = "Not Extended";
	status_msgs[511] = "Network Authentication Required";
}

void add_default_pages() {

default_pages[301] =
"<html>" CRLF
"<head><title>301 Moved Permanently</title></head>" CRLF
"<body>" CRLF
"<center><h1>301 Moved Permanently</h1></center>" CRLF
"<hr><center>" WEBSERV_V "</center>" CRLF
"</body>" CRLF
"</html>";


default_pages[302] =
"<html>" CRLF
"<head><title>302 Found</title></head>" CRLF
"<body>" CRLF
"<center><h1>302 Found</h1></center>" CRLF
"<hr><center>" WEBSERV_V "</center>" CRLF
"</body>" CRLF
"</html>";


default_pages[303] =
"<html>" CRLF
"<head><title>303 See Other</title></head>" CRLF
"<body>" CRLF
"<center><h1>303 See Other</h1></center>" CRLF
"<hr><center>" WEBSERV_V "</center>" CRLF
"</body>" CRLF
"</html>";


default_pages[307] =
"<html>" CRLF
"<head><title>307 Temporary Redirect</title></head>" CRLF
"<body>" CRLF
"<center><h1>307 Temporary Redirect</h1></center>" CRLF
"<hr><center>" WEBSERV_V "</center>" CRLF
"</body>" CRLF
"</html>";


default_pages[308] =
"<html>" CRLF
"<head><title>308 Permanent Redirect</title></head>" CRLF
"<body>" CRLF
"<center><h1>308 Permanent Redirect</h1></center>" CRLF
"<hr><center>" WEBSERV_V "</center>" CRLF
"</body>" CRLF
"</html>";


default_pages[400] =
"<html>" CRLF
"<head><title>400 Bad Request</title></head>" CRLF
"<body>" CRLF
"<center><h1>400 Bad Request</h1></center>" CRLF
"<hr><center>" WEBSERV_V "</center>" CRLF
"</body>" CRLF
"</html>";


default_pages[401] =
"<html>" CRLF
"<head><title>401 Authorization Required</title></head>" CRLF
"<body>" CRLF
"<center><h1>401 Authorization Required</h1></center>" CRLF
"<hr><center>" WEBSERV_V "</center>" CRLF
"</body>" CRLF
"</html>";


default_pages[402] =
"<html>" CRLF
"<head><title>402 Payment Required</title></head>" CRLF
"<body>" CRLF
"<center><h1>402 Payment Required</h1></center>" CRLF
"<hr><center>" WEBSERV_V "</center>" CRLF
"</body>" CRLF
"</html>";


default_pages[403] =
"<html>" CRLF
"<head><title>403 Forbidden</title></head>" CRLF
"<body>" CRLF
"<center><h1>403 Forbidden</h1></center>" CRLF
"<hr><center>" WEBSERV_V "</center>" CRLF
"</body>" CRLF
"</html>";


default_pages[404] =
"<html>" CRLF
"<head><title>404 Not Found</title></head>" CRLF
"<body>" CRLF
"<center><h1>404 Not Found</h1></center>" CRLF
"<hr><center>" WEBSERV_V "</center>" CRLF
"</body>" CRLF
"</html>";


default_pages[405] =
"<html>" CRLF
"<head><title>405 Not Allowed</title></head>" CRLF
"<body>" CRLF
"<center><h1>405 Not Allowed</h1></center>" CRLF
"<hr><center>" WEBSERV_V "</center>" CRLF
"</body>" CRLF
"</html>";


default_pages[406] =
"<html>" CRLF
"<head><title>406 Not Acceptable</title></head>" CRLF
"<body>" CRLF
"<center><h1>406 Not Acceptable</h1></center>" CRLF
"<hr><center>" WEBSERV_V "</center>" CRLF
"</body>" CRLF
"</html>";


default_pages[408] =
"<html>" CRLF
"<head><title>408 Request Time-out</title></head>" CRLF
"<body>" CRLF
"<center><h1>408 Request Time-out</h1></center>" CRLF
"<hr><center>" WEBSERV_V "</center>" CRLF
"</body>" CRLF
"</html>";


default_pages[409] =
"<html>" CRLF
"<head><title>409 Conflict</title></head>" CRLF
"<body>" CRLF
"<center><h1>409 Conflict</h1></center>" CRLF
"<hr><center>" WEBSERV_V "</center>" CRLF
"</body>" CRLF
"</html>";


default_pages[410] =
"<html>" CRLF
"<head><title>410 Gone</title></head>" CRLF
"<body>" CRLF
"<center><h1>410 Gone</h1></center>" CRLF
"<hr><center>" WEBSERV_V "</center>" CRLF
"</body>" CRLF
"</html>";


default_pages[411] =
"<html>" CRLF
"<head><title>411 Length Required</title></head>" CRLF
"<body>" CRLF
"<center><h1>411 Length Required</h1></center>" CRLF
"<hr><center>" WEBSERV_V "</center>" CRLF
"</body>" CRLF
"</html>";


default_pages[412] =
"<html>" CRLF
"<head><title>412 Precondition Failed</title></head>" CRLF
"<body>" CRLF
"<center><h1>412 Precondition Failed</h1></center>" CRLF
"<hr><center>" WEBSERV_V "</center>" CRLF
"</body>" CRLF
"</html>";


default_pages[413] =
"<html>" CRLF
"<head><title>413 Request Entity Too Large</title></head>" CRLF
"<body>" CRLF
"<center><h1>413 Request Entity Too Large</h1></center>" CRLF
"<hr><center>" WEBSERV_V "</center>" CRLF
"</body>" CRLF
"</html>";


default_pages[414] =
"<html>" CRLF
"<head><title>414 Request-URI Too Large</title></head>" CRLF
"<body>" CRLF
"<center><h1>414 Request-URI Too Large</h1></center>" CRLF
"<hr><center>" WEBSERV_V "</center>" CRLF
"</body>" CRLF
"</html>";


default_pages[415] =
"<html>" CRLF
"<head><title>415 Unsupported Media Type</title></head>" CRLF
"<body>" CRLF
"<center><h1>415 Unsupported Media Type</h1></center>" CRLF
"<hr><center>" WEBSERV_V "</center>" CRLF
"</body>" CRLF
"</html>";


default_pages[416] =
"<html>" CRLF
"<head><title>416 Requested Range Not Satisfiable</title></head>" CRLF
"<body>" CRLF
"<center><h1>416 Requested Range Not Satisfiable</h1></center>" CRLF
"<hr><center>" WEBSERV_V "</center>" CRLF
"</body>" CRLF
"</html>";


default_pages[421] =
"<html>" CRLF
"<head><title>421 Misdirected Request</title></head>" CRLF
"<body>" CRLF
"<center><h1>421 Misdirected Request</h1></center>" CRLF
"<hr><center>" WEBSERV_V "</center>" CRLF
"</body>" CRLF
"</html>";


default_pages[429] =
"<html>" CRLF
"<head><title>429 Too Many Requests</title></head>" CRLF
"<body>" CRLF
"<center><h1>429 Too Many Requests</h1></center>" CRLF
"<hr><center>" WEBSERV_V "</center>" CRLF
"</body>" CRLF
"</html>";


default_pages[494] =
"<html>" CRLF
"<head><title>400 Request Header Or Cookie Too Large</title></head>"
CRLF
"<body>" CRLF
"<center><h1>400 Bad Request</h1></center>" CRLF
"<center>Request Header Or Cookie Too Large</center>" CRLF
"<hr><center>" WEBSERV_V "</center>" CRLF
"</body>" CRLF
"</html>";


default_pages[495] =
"<html>" CRLF
"<head><title>400 The SSL certificate error</title></head>"
CRLF
"<body>" CRLF
"<center><h1>400 Bad Request</h1></center>" CRLF
"<center>The SSL certificate error</center>" CRLF
"<hr><center>" WEBSERV_V "</center>" CRLF
"</body>" CRLF
"</html>";


default_pages[496] =
"<html>" CRLF
"<head><title>400 No required SSL certificate was sent</title></head>"
CRLF
"<body>" CRLF
"<center><h1>400 Bad Request</h1></center>" CRLF
"<center>No required SSL certificate was sent</center>" CRLF
"<hr><center>" WEBSERV_V "</center>" CRLF
"</body>" CRLF
"</html>";


default_pages[497] =
"<html>" CRLF
"<head><title>400 The plain HTTP request was sent to HTTPS port</title></head>"
CRLF
"<body>" CRLF
"<center><h1>400 Bad Request</h1></center>" CRLF
"<center>The plain HTTP request was sent to HTTPS port</center>" CRLF
"<hr><center>" WEBSERV_V "</center>" CRLF
"</body>" CRLF
"</html>";


default_pages[500] =
"<html>" CRLF
"<head><title>500 Internal Server Error</title></head>" CRLF
"<body>" CRLF
"<center><h1>500 Internal Server Error</h1></center>" CRLF
"<hr><center>" WEBSERV_V "</center>" CRLF
"</body>" CRLF
"</html>";


default_pages[501] =
"<html>" CRLF
"<head><title>501 Not Implemented</title></head>" CRLF
"<body>" CRLF
"<center><h1>501 Not Implemented</h1></center>" CRLF
"<hr><center>" WEBSERV_V "</center>" CRLF
"</body>" CRLF
"</html>";


default_pages[502] =
"<html>" CRLF
"<head><title>502 Bad Gateway</title></head>" CRLF
"<body>" CRLF
"<center><h1>502 Bad Gateway</h1></center>" CRLF
"<hr><center>" WEBSERV_V "</center>" CRLF
"</body>" CRLF
"</html>";


default_pages[503] =
"<html>" CRLF
"<head><title>503 Service Temporarily Unavailable</title></head>" CRLF
"<body>" CRLF
"<center><h1>503 Service Temporarily Unavailable</h1></center>" CRLF
"<hr><center>" WEBSERV_V "</center>" CRLF
"</body>" CRLF
"</html>";


default_pages[504] =
"<html>" CRLF
"<head><title>504 Gateway Time-out</title></head>" CRLF
"<body>" CRLF
"<center><h1>504 Gateway Time-out</h1></center>" CRLF
"<hr><center>" WEBSERV_V "</center>" CRLF
"</body>" CRLF
"</html>";


default_pages[505] =
"<html>" CRLF
"<head><title>505 HTTP Version Not Supported</title></head>" CRLF
"<body>" CRLF
"<center><h1>505 HTTP Version Not Supported</h1></center>" CRLF
"<hr><center>" WEBSERV_V "</center>" CRLF
"</body>" CRLF
"</html>";


default_pages[507] =
"<html>" CRLF
"<head><title>507 Insufficient Storage</title></head>" CRLF
"<body>" CRLF
"<center><h1>507 Insufficient Storage</h1></center>" CRLF
"<hr><center>" WEBSERV_V "</center>" CRLF
"</body>" CRLF
"</html>";

}

void add_mime_types() {
	mime_types["html"] =	"text/html";
	mime_types["htm"] =		"text/html";
	mime_types["shtml"] =	"text/html";
	mime_types["css"] =		"text/css";
	mime_types["xml"] =		"text/xml";
	mime_types["gif"] =		"image/gif";
	mime_types["jpeg"] =	"image/jpeg";
	mime_types["jpg"] =		"image/jpeg";
	mime_types["js"] =		"application/javascript";
	mime_types["atom"] =	"application/atom+xml";
	mime_types["rss"] =		"application/rss+xml";
	mime_types["mml"] =		"text/mathml";
	mime_types["txt"] =		"text/plain";
	mime_types["jad"] =		"text/vnd.sun.j2me.app-descriptor";
	mime_types["wml"] =		"text/vnd.wap.wml";
	mime_types["htc"] =		"text/x-component";
	mime_types["avif"] =	"image/avif";
	mime_types["png"] =		"image/png";
	mime_types["svg"] =		"image/svg+xml";
	mime_types["svgz"] =	"image/svg+xml";
	mime_types["tif"] =		"image/tiff";
	mime_types["tiff"] =	"image/tiff";
	mime_types["wbmp"] =	"image/vnd.wap.wbmp";
	mime_types["webp"] =	"image/webp";
	mime_types["ico"] =		"image/x-icon";
	mime_types["jng"] =		"image/x-jng";
	mime_types["bmp"] =		"image/x-ms-bmp";
	mime_types["woff"] =	"font/woff";
	mime_types["woff2"] =	"font/woff2";
	mime_types["jar"] =		"application/java-archive";
	mime_types["war"] =		"application/java-archive";
	mime_types["ear"] =		"application/java-archive";
	mime_types["json"] =	"application/json";
	mime_types["hqx"] =		"application/mac-binhex40";
	mime_types["doc"] =		"application/msword";
	mime_types["pdf"] =		"application/pdf";
	mime_types["ps"] =		"application/postscript";
	mime_types["eps"] =		"application/postscript";
	mime_types["ai"] =		"application/postscript";
	mime_types["rtf"] =		"application/rtf";
	mime_types["m3u8"] =	"application/vnd.apple.mpegurl";
	mime_types["kml"] =		"application/vnd.google-earth.kml+xml";
	mime_types["kmz"] =		"application/vnd.google-earth.kmz";
	mime_types["xls"] =		"application/vnd.ms-excel";
	mime_types["eot"] =		"application/vnd.ms-fontobject";
	mime_types["ppt"] =		"application/vnd.ms-powerpoint";
	mime_types["odg"] =		"application/vnd.oasis.opendocument.graphics";
	mime_types["odp"] =		"application/vnd.oasis.opendocument.presentation";
	mime_types["ods"] =		"application/vnd.oasis.opendocument.spreadsheet";
	mime_types["odt"] =		"application/vnd.oasis.opendocument.text";
	mime_types["wmlc"] =	"application/vnd.wap.wmlc";
	mime_types["wasm"] =	"application/wasm";
	mime_types["7z"] =		"application/x-7z-compressed";
	mime_types["cco"] =		"application/x-cocoa";
	mime_types["jardiff"] =	"application/x-java-archive-diff";
	mime_types["jnlp"] =	"application/x-java-jnlp-file";
	mime_types["run"] =		"application/x-makeself";
	mime_types["pl"] =		"application/x-perl";
	mime_types["pm"] =		"application/x-perl";
	mime_types["prc"] =		"application/x-pilot";
	mime_types["pdb"] =		"application/x-pilot";
	mime_types["rar"] =		"application/x-rar-compressed";
	mime_types["rpm"] =		"application/x-redhat-package-manager";
	mime_types["sea"] =		"application/x-sea";
	mime_types["swf"] =		"application/x-shockwave-flash";
	mime_types["sit"] =		"application/x-stuffit";
	mime_types["tcl"] =		"application/x-tcl";
	mime_types["tk"] =		"application/x-tcl";
	mime_types["der"] =		"application/x-x509-ca-cert";
	mime_types["pem"] =		"application/x-x509-ca-cert";
	mime_types["crt"] =		"application/x-x509-ca-cert";
	mime_types["xpi"] =		"application/x-xpinstall";
	mime_types["xhtml"] =	"application/xhtml+xml";
	mime_types["xspf"] =	"application/xspf+xml";
	mime_types["zip"] =		"application/zip";
	mime_types["bin"] =		"application/octet-stream";
	mime_types["exe"] =		"application/octet-stream";
	mime_types["dll"] =		"application/octet-stream";
	mime_types["deb"] =		"application/octet-stream";
	mime_types["dmg"] =		"application/octet-stream";
	mime_types["iso"] =		"application/octet-stream";
	mime_types["img"] =		"application/octet-stream";
	mime_types["msi"] =		"application/octet-stream";
	mime_types["msp"] =		"application/octet-stream";
	mime_types["msm"] =		"application/octet-stream";
	mime_types["mid"] =		"audio/midi";
	mime_types["midi"] =	"audio/midi";
	mime_types["kar"] =		"audio/midi";
	mime_types["mp3"] =		"audio/mpeg";
	mime_types["ogg"] =		"audio/ogg";
	mime_types["m4a"] =		"audio/x-m4a";
	mime_types["ra"] =		"audio/x-realaudio";
	mime_types["3gpp"] =	"video/3gpp";
	mime_types["3gp"] =		"video/3gpp";
	mime_types["ts"] =		"video/mp2t";
	mime_types["mp4"] =		"video/mp4";
	mime_types["mpeg"] =	"video/mpeg";
	mime_types["mpg"] =		"video/mpeg";
	mime_types["mov"] =		"video/quicktime";
	mime_types["webm"] =	"video/webm";
	mime_types["flv"] =		"video/x-flv";
	mime_types["m4v"] =		"video/x-m4v";
	mime_types["mng"] =		"video/x-mng";
	mime_types["asx"] =		"video/x-ms-asf";
	mime_types["asf"] =		"video/x-ms-asf";
	mime_types["wmv"] =		"video/x-ms-wmv";
	mime_types["avi"] =		"video/x-msvideo";
	mime_types["pptx"] = "application/vnd.openxmlformats-officedocument.presentationml.presentation";
	mime_types["xlsx"] = "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet";
	mime_types["docx"] = "application/vnd.openxmlformats-officedocument.wordprocessingml.document";
}
