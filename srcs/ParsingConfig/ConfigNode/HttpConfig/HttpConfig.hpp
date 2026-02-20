#ifndef CONFIG_HTTP_HPP
#define CONFIG_HTTP_HPP

#include <iostream>
#include "AConfigNode.hpp"
#include "ServerConfig.hpp"

class HttpConfig : public AConfigNode
{
	private:
		Vec<ServerConfig> servers;

	public:
		HttpConfig();
		~HttpConfig();

		const Vec<ServerConfig> &getServers() const;
		Vec<ServerConfig>		&getServers();
};

std::ostream &operator<<(std::ostream &os, const HttpConfig &data);

#endif