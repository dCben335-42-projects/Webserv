#ifndef CONFIG_NODE_HTTP_HPP
#define CONFIG_NODE_HTTP_HPP

#include <iostream>
#include "ServerNode.hpp"

class HttpNode : public AConfigNode
{
	private:
		Vec<ServerNode> servers;

	public:
		HttpNode();
		~HttpNode();

		const Vec<ServerNode> &getServers() const;
		Vec<ServerNode>		  &getServers();

		void addServer(const ServerNode &server);
};

std::ostream &operator<<(std::ostream &os, const HttpNode &data);

#endif