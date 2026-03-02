#include "HttpNode.hpp"
#include "Logger.hpp"

HttpNode::HttpNode()
{
}

HttpNode::~HttpNode()
{
}

const Vec<ServerNode> &HttpNode::getServers() const
{
	return servers;
}

Vec<ServerNode> &HttpNode::getServers()
{
	return servers;
}

void HttpNode::addServer(const ServerNode &server)
{
	servers.push_back(server);
}


std::ostream &operator<<(std::ostream &os, const HttpNode &data)
{
	os << "HttpNode:\n";
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