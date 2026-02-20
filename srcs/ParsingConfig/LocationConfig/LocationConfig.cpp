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

const LocationConfig::Directives &LocationConfig::getDirectives() const
{
	return directives;
}

const Map<std::string> &LocationConfig::getAddHeaders() const
{
	return add_headers;
}

const Map<std::string> &LocationConfig::getCgiPass() const
{
	return cgi_pass;
}

const Map<std::string> &LocationConfig::getErrorPages() const
{
	return error_pages;
}

// ===== SETTERS =====
void LocationConfig::setDirectiveAt(const std::string &key, const Vec<std::string> &value)
{
	directives[key] = value;
}

void LocationConfig::setCgiPassAt(const std::string &key, const std::string &value)
{
	cgi_pass[key] = value;
}

void LocationConfig::setErrorPagesAt(const std::string &key, const std::string &value)
{
	error_pages[key] = value;
}

void LocationConfig::setAddHeadersAt(const std::string &key, const std::string &value)
{
	add_headers[key] = value;
}