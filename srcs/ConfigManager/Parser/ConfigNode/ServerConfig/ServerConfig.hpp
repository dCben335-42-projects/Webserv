#ifndef CONFIG_SERVER_HPP
#define CONFIG_SERVER_HPP

#include <iostream>
#include "AConfigNode.hpp"
#include "LocationConfig.hpp"
#include "SocketAddress.hpp"

class ServerConfig : public AConfigNode
{
	private:
		SocketAddress		m_socketAddress;
		unsigned short		m_port;
		Map<LocationConfig> m_locations;

	public:
		ServerConfig();
		~ServerConfig();

		const SocketAddress &getSocketAddress() const;
		unsigned short		 getPort() const;

		void setSocketAddress(const SocketAddress &socketAddress);
		void setPort(const unsigned short &port);

		const Map<LocationConfig> &getLocations() const;
		Map<LocationConfig>		  &getLocations();

		void setLocation(const std::string &key, const LocationConfig &location);
};

std::ostream &operator<<(std::ostream &os, const ServerConfig &data);

#endif