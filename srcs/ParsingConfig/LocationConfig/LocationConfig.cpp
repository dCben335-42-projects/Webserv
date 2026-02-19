#include "LocationConfig.hpp"

std::ostream &operator<<(std::ostream &os, const LocationConfig &data)
{
	Logger::incrementIndent();
	// os << Logger::get(data.path) << ": ";
	os << Logger::get(data.directives) << "\n";
	if (!data.add_headers.empty())
		os << Logger::indent() << "add_headers: " << Logger::get(data.add_headers) << "\n";
	if (!data.cgi_pass.empty())
		os << Logger::indent() << "cgi_pass: " << Logger::get(data.cgi_pass) << "\n";
	if (!data.error_pages.empty())
		os << Logger::indent() << "error_pages: " << Logger::get(data.error_pages) << "\n";
	Logger::decrementIndent();
	return os;
}