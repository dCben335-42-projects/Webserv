#ifndef CONFIG_LOCATION_HPP
#define CONFIG_LOCATION_HPP

#include <string>
#include <iostream>
#include "AConfigNode.hpp"

class LocationConfig : public AConfigNode
{
	private:
		std::string path;

	public:
		LocationConfig();
		~LocationConfig();

		const std::string &getPath() const;
		std::string		  &getPath();
};

std::ostream &operator<<(std::ostream &os, const LocationConfig &data);

#endif