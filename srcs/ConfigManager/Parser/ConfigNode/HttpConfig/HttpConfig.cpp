#include "HttpConfig.hpp"
#include "Logger.hpp"

HttpConfig::HttpConfig()
{
}

HttpConfig::~HttpConfig()
{
}

const Vec<ServerConfig> &HttpConfig::getServers() const
{
	return servers;
}

Vec<ServerConfig> &HttpConfig::getServers()
{
	return servers;
}

void HttpConfig::addServer(const ServerConfig &server)
{
	servers.push_back(server);
}


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