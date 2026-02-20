#ifndef CONFIG_SERVER_HPP
#define CONFIG_SERVER_HPP

#include <iostream>
#include "AConfigNode.hpp"
#include "LocationConfig.hpp"

class ServerConfig : public AConfigNode
{
	private:
		Map<LocationConfig> locations;

	public:
		ServerConfig();
		~ServerConfig();

		const Map<LocationConfig> &getLocations() const;
		Map<LocationConfig>		  &getLocations();
};

std::ostream &operator<<(std::ostream &os, const ServerConfig &data);

#endif