

#include "LocationNode.hpp"
#include "Logger.hpp"

LocationNode::LocationNode()
{
}

LocationNode::~LocationNode()
{
}

// ===== GETTERS =====
const std::string &LocationNode::getPath() const
{
	return path;
}

std::string &LocationNode::getPath()
{
	return path;
}


std::ostream &operator<<(std::ostream &os, const LocationNode &data)
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