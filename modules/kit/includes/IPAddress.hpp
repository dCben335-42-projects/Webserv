#ifndef KIT_IP_ADDRESS_HPP
#define KIT_IP_ADDRESS_HPP

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

		static bool extractPort(
			const std::string &ip,
			unsigned short	  &port,
			size_t			  &startPos,
			bool			  &invalidPort,
			ssize_t			   closeBracketPos);

		static bool isIPv6(const std::string &ip);

		static struct in6_addr convertIPv6(const std::string &ip);
		static uint32_t		   convertIPv4(const std::string &ip);

		static struct addrinfo *resolveHost(const std::string &hostname);
};

#endif
