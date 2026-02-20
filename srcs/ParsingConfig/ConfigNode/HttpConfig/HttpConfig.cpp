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
const Vec<ServerConfig> &HttpConfig::getServers() const
{
	return servers;
}

Vec<ServerConfig> &HttpConfig::getServers()
{
	return servers;
}