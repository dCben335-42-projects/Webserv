#ifndef KIT_IP_ADDRESS_HPP
#define KIT_IP_ADDRESS_HPP

#pragma once

#include <string>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <cstring>
#include <stdexcept>

class IPAddress
{
	private:
		IPAddress();

		static struct in6_addr buildIPv6(const uint16_t (&parts)[8]);

		static void expandCompressed(
			uint16_t (&parts)[8],
			const int partIndex,
			const int compressIndex);
		static void parseIPv6Parts(
			const std::string &ip,
			uint16_t (&parts)[8],
			int &partIndex,
			int &compressIndex);

	public:
		static bool isHostname(const std::string &ip);
		static bool isIPv4(const std::string &ip);
		static bool isIPv6(const std::string &ip);

		static struct in6_addr convertIPv6(const std::string &ip);
		static uint32_t		   convertIPv4(const std::string &ip);

		static struct addrinfo *resolveHost(const std::string &hostname);
};

#endif // KIT_IP_ADDRESS_HPP
