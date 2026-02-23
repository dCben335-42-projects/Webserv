#ifndef CONFIG_SERVER_HPP
#define CONFIG_SERVER_HPP

#include <iostream>
#include "AConfigNode.hpp"
#include "LocationConfig.hpp"

class ServerConfig : public AConfigNode
{
	private:
		std::string			m_ip;
		std::string			m_port;
		Map<LocationConfig> m_locations;

	public:
		ServerConfig();
		~ServerConfig();

		const std::string &getIp() const;
		const std::string &getPort() const;

		void setIp(const std::string &ip);
		void setPort(const std::string &port);

		const Map<LocationConfig> &getLocations() const;
		Map<LocationConfig>		  &getLocations();

		void setLocation(const std::string &key, const LocationConfig &location);
};

std::ostream &operator<<(std::ostream &os, const ServerConfig &data);

#endif