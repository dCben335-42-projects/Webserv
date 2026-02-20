#include "ServerConfig.hpp"
#include "Logger.hpp"

std::ostream &operator<<(std::ostream &os, const ServerConfig &data)
{
	os << Logger::get(data.getDirectives()) << "\n";
	os << Logger::indent() << "locations: " << Logger::get(data.getLocations()) << "\n";
	if (!data.getAddHeaders().empty())
		os << Logger::indent() << "add_headers: " << Logger::get(data.getAddHeaders()) << "\n";
	if (!data.getCgiPass().empty())
		os << Logger::indent() << "cgi_pass: " << Logger::get(data.getCgiPass()) << "\n";
	if (!data.getErrorPages().empty())
		os << Logger::indent() << "error_pages: " << Logger::get(data.getErrorPages()) << "\n";
	return os;
}

// ===== CONSTRUCTOR =====
ServerConfig::ServerConfig()
{
}

ServerConfig::~ServerConfig()
{
}

// ===== GETTERS =====
const ServerConfig::Directives &ServerConfig::getDirectives() const
{
	return directives;
}

const Map<LocationConfig> &ServerConfig::getLocations() const
{
	return locations;
}

Map<LocationConfig> &ServerConfig::getLocations()
{
	return locations;
}

const Map<std::string> &ServerConfig::getAddHeaders() const
{
	return add_headers;
}

const Map<std::string> &ServerConfig::getCgiPass() const
{
	return cgi_pass;
}

const Map<std::string> &ServerConfig::getErrorPages() const
{
	return error_pages;
}

// ===== SETTERS =====
void ServerConfig::setDirectiveAt(const std::string &key, const Vec<std::string> &value)
{
	directives[key] = value;
}

void ServerConfig::setCgiPassAt(const std::string &key, const std::string &value)
{
	cgi_pass[key] = value;
}

void ServerConfig::setErrorPagesAt(const std::string &key, const std::string &value)
{
	error_pages[key] = value;
}

void ServerConfig::setAddHeadersAt(const std::string &key, const std::string &value)
{
	add_headers[key] = value;
}