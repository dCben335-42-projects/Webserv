#include "LocationConfig.hpp"
#include "Logger.hpp"

LocationConfig::LocationConfig()
{
}

LocationConfig::~LocationConfig()
{
}

// ===== GETTERS =====
const std::string &LocationConfig::getPath() const
{
	return path;
}

std::string &LocationConfig::getPath()
{
	return path;
}


std::ostream &operator<<(std::ostream &os, const LocationConfig &data)
{
	Logger::incrementIndent();
	os << Logger::get(data.getDirectives()) << "\n";
	if (!data.getAddHeaders().empty())
		os << Logger::indent() << "m_addHeaders: " << Logger::get(data.getAddHeaders()) << "\n";
	if (!data.getCgiPass().empty())
		os << Logger::indent() << "m_cgiPass: " << Logger::get(data.getCgiPass()) << "\n";
	if (!data.getErrorPages().empty())
		os << Logger::indent() << "m_errorPages: " << Logger::get(data.getErrorPages()) << "\n";
	Logger::decrementIndent();
	return os;
}