#include "HttpConfig.hpp"

std::ostream &operator<<(std::ostream &os, const HttpConfig &data)
{
	os << "HttpConfig:\n";
	os << Logger::get(data.directives) << "\n";
	os << Logger::indent() << "servers: " << Logger::get(data.servers) << "\n";
	if (!data.add_headers.empty())
		os << Logger::indent() << "add_headers: " << Logger::get(data.add_headers) << "\n";
	if (!data.cgi_pass.empty())
		os << Logger::indent() << "cgi_pass: " << Logger::get(data.cgi_pass) << "\n";
	if (!data.error_pages.empty())
		os << Logger::indent() << "error_pages: " << Logger::get(data.error_pages) << "\n";
	return os;
}