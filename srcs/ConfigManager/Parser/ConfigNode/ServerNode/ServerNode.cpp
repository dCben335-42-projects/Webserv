#include "ServerNode.hpp"
#include "Logger.hpp"


ServerNode::ServerNode()
{
}

ServerNode::~ServerNode()
{
}

const Map<LocationNode> &ServerNode::getLocations() const
{
	return m_locations;
}


const SocketAddress &ServerNode::getSocketAddress() const
{
	return m_socketAddress;
}

unsigned short ServerNode::getPort() const
{
	return m_port;
}


void ServerNode::setSocketAddress(const SocketAddress &socketAddress)
{
	m_socketAddress = socketAddress;
}

void ServerNode::setPort(const unsigned short &port)
{
	m_port = port;
}


void ServerNode::setLocation(const std::string &key, const LocationNode &location)
{
	m_locations.set(key, location);
}


std::ostream &operator<<(std::ostream &os, const ServerNode &data)
{
	os << Logger::get(data.getDirectives()) << "\n";
	const Map<LocationNode> &locations = data.getLocations();
	if (!locations.empty())
		os << Logger::indent() << "locations: " << Logger::get(locations) << "\n";
	if (!data.getAddHeaders().empty())
		os << Logger::indent() << "m_addHeaders: " << Logger::get(data.getAddHeaders()) << "\n";
	if (!data.getCgiPass().empty())
		os << Logger::indent() << "m_cgiPass: " << Logger::get(data.getCgiPass()) << "\n";
	if (!data.getErrorPages().empty())
		os << Logger::indent() << "m_errorPages: " << Logger::get(data.getErrorPages()) << "\n";
	return os;
}
