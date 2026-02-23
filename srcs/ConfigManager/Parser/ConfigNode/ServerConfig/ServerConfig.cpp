#include "ServerConfig.hpp"
#include "Logger.hpp"


ServerConfig::ServerConfig()
{
}

ServerConfig::~ServerConfig()
{
}

const Map<LocationConfig> &ServerConfig::getLocations() const
{
	return m_locations;
}



const std::string &ServerConfig::getIp() const
{
	return m_ip;
}

const std::string &ServerConfig::getPort() const
{
	return m_port;
}


void ServerConfig::setIp(const std::string &ip)
{
	m_ip = ip;
}

void ServerConfig::setPort(const std::string &port)
{
	m_port = port;
}



// ===== SETTERS =====
void ServerConfig::setLocation(const std::string &key, const LocationConfig &location)
{
	m_locations.set(key, location);
}


std::ostream &operator<<(std::ostream &os, const ServerConfig &data)
{
	os << Logger::get(data.getDirectives()) << "\n";
	os << Logger::indent() << "locations: " << Logger::get(data.getLocations()) << "\n";
	if (!data.getAddHeaders().empty())
		os << Logger::indent() << "m_addHeaders: " << Logger::get(data.getAddHeaders()) << "\n";
	if (!data.getCgiPass().empty())
		os << Logger::indent() << "m_cgiPass: " << Logger::get(data.getCgiPass()) << "\n";
	if (!data.getErrorPages().empty())
		os << Logger::indent() << "m_errorPages: " << Logger::get(data.getErrorPages()) << "\n";
	return os;
}
