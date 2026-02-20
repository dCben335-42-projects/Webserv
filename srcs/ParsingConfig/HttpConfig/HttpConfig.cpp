#include "HttpConfig.hpp"
#include "Logger.hpp"

std::ostream &operator<<(std::ostream &os, const HttpConfig &data)
{
	os << "HttpConfig:\n";
	os << Logger::get(data.getDirectives()) << "\n";
	os << Logger::indent() << "servers: " << Logger::get(data.getServers()) << "\n";
	if (!data.getAddHeaders().empty())
		os << Logger::indent() << "add_headers: " << Logger::get(data.getAddHeaders()) << "\n";
	if (!data.getCgiPass().empty())
		os << Logger::indent() << "cgi_pass: " << Logger::get(data.getCgiPass()) << "\n";
	if (!data.getErrorPages().empty())
		os << Logger::indent() << "error_pages: " << Logger::get(data.getErrorPages()) << "\n";
	return os;
}

// ===== CONSTRUCTOR =====
HttpConfig::HttpConfig()
{
}
HttpConfig::~HttpConfig()
{
}

// ===== GETTERS =====
const HttpConfig::Directives &HttpConfig::getDirectives() const
{
	return directives;
}

const Vec<ServerConfig> &HttpConfig::getServers() const
{
	return servers;
}

Vec<ServerConfig> &HttpConfig::getServers()
{
	return servers;
}

const Map<std::string> &HttpConfig::getAddHeaders() const
{
	return add_headers;
}

const Map<std::string> &HttpConfig::getCgiPass() const
{
	return cgi_pass;
}

const Map<std::string> &HttpConfig::getErrorPages() const
{
	return error_pages;
}

// ===== SETTERS =====
void HttpConfig::setDirectiveAt(const std::string &key, const Vec<std::string> &value)
{
	directives[key] = value;
}

void HttpConfig::setCgiPassAt(const std::string &key, const std::string &value)
{
	cgi_pass[key] = value;
}

void HttpConfig::setErrorPagesAt(const std::string &key, const std::string &value)
{
	error_pages[key] = value;
}

void HttpConfig::setAddHeadersAt(const std::string &key, const std::string &value)
{
	add_headers[key] = value;
}