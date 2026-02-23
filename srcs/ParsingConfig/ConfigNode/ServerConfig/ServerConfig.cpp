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
const Map<LocationConfig> &ServerConfig::getLocations() const
{
	return locations;
}

Map<LocationConfig> &ServerConfig::getLocations()
{
	return locations;
}