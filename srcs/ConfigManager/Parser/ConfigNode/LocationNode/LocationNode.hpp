#ifndef CONFIG_NODE_LOCATION_HPP
#define CONFIG_NODE_LOCATION_HPP

#include <string>
#include "AConfigNode.hpp"

class LocationNode : public AConfigNode
{
	private:
		std::string path;

	public:
		LocationNode();
		~LocationNode();

		const std::string &getPath() const;
		std::string		  &getPath();
};

std::ostream &operator<<(std::ostream &os, const LocationNode &data);

#endif