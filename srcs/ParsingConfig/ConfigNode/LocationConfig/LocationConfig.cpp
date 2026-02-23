#include "LocationConfig.hpp"
#include "Logger.hpp"

std::ostream &operator<<(std::ostream &os, const LocationConfig &data)
{
	Logger::incrementIndent();
	os << Logger::get(data.getDirectives()) << "\n";
	if (!data.getAddHeaders().empty())
		os << Logger::indent() << "add_headers: " << Logger::get(data.getAddHeaders()) << "\n";
	if (!data.getCgiPass().empty())
		os << Logger::indent() << "cgi_pass: " << Logger::get(data.getCgiPass()) << "\n";
	if (!data.getErrorPages().empty())
		os << Logger::indent() << "error_pages: " << Logger::get(data.getErrorPages()) << "\n";
	Logger::decrementIndent();
	return os;
}

// ===== CONSTRUCTOR =====
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