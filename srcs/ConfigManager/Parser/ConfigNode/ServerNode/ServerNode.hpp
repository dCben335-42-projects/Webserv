#ifndef CONFIG_NODE_SERVER_HPP
#define CONFIG_NODE_SERVER_HPP

#include <iostream>
#include "AConfigNode.hpp"
#include "LocationNode.hpp"
#include "SocketAddress.hpp"

class ServerNode : public AConfigNode
{
	private:
		SocketAddress	  m_socketAddress;
		unsigned short	  m_port;
		Map<LocationNode> m_locations;

	public:
		ServerNode();
		~ServerNode();

		const SocketAddress &getSocketAddress() const;
		unsigned short		 getPort() const;

		void setSocketAddress(const SocketAddress &socketAddress);
		void setPort(const unsigned short &port);

		const Map<LocationNode> &getLocations() const;
		Map<LocationNode>		&getLocations();

		void setLocation(const std::string &key, const LocationNode &location);
};

std::ostream &operator<<(std::ostream &os, const ServerNode &data);

#endif