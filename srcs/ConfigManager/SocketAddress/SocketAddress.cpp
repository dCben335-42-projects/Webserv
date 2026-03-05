#include "SocketAddress.hpp"
#include "IPAddress.hpp"
#include <cctype>
#include "Logger.hpp"
#include "String.hpp"
#include <exception>


SocketAddress::SocketAddress()
{
	std::memset(&m_storage, 0, sizeof(m_storage));
	m_length = 0;
	m_isIPv6 = false;
	m_port	 = 0;
	m_ip	 = "";
}

SocketAddress::SocketAddress(const std::string &ip, const unsigned short port)
{
	m_port = port;
	m_ip   = ip;
	initIP(ip, port);
}

SocketAddress::SocketAddress(const SocketAddress &other)
{
	std::memcpy(&m_storage, &other.m_storage, sizeof(m_storage));
	m_length = other.m_length;
	m_isIPv6 = other.m_isIPv6;
	m_port	 = other.m_port;
	m_ip	 = other.m_ip;
}

void SocketAddress::initIP(const std::string &ip, const unsigned short port)
{
	std::memset(&m_storage, 0, sizeof(m_storage));

	m_isHostname = IPAddress::isHostname(ip);
	if (m_isHostname)
	{
		struct addrinfo *result = IPAddress::resolveHost(ip);
		m_isIPv6				= result->ai_family == AF_INET6;
		if (m_isIPv6)
			initIPv6(port, reinterpret_cast<struct sockaddr_in6 *>(result->ai_addr)->sin6_addr);
		else
			initIPv4(port, reinterpret_cast<struct sockaddr_in *>(result->ai_addr)->sin_addr.s_addr);
		freeaddrinfo(result);
		return;
	}
	m_isIPv6 = IPAddress::isIPv6(ip);
	if (m_isIPv6)
	{
		const std::string ipWithoutBrackets = ip.substr(1, ip.size() - 2);
		initIPv6(port, IPAddress::convertIPv6(ipWithoutBrackets));
		return;
	}
	const Vec<std::string> splittedIp = String::split(ip,  ".");
	for (size_t i = 0; i < splittedIp.size(); i++)
	{
		if (Number::toInt(splittedIp[i]) > 255)
			throw std::runtime_error("Invalid IP address");
	}
	initIPv4(port, IPAddress::convertIPv4(ip));
}


void SocketAddress::initIPv4(const unsigned short port, const uint32_t sin_addr)
{
	struct sockaddr_in *addr = reinterpret_cast<struct sockaddr_in *>(&m_storage);

	addr->sin_family	  = AF_INET;
	addr->sin_port		  = htons(port);
	addr->sin_addr.s_addr = sin_addr;
	m_length			  = sizeof(struct sockaddr_in);
}


void SocketAddress::initIPv6(const unsigned short port, const in6_addr sin6_addr)
{
	struct sockaddr_in6 *addr = reinterpret_cast<struct sockaddr_in6 *>(&m_storage);

	addr->sin6_family = AF_INET6;
	addr->sin6_port	  = htons(port);
	addr->sin6_addr	  = sin6_addr;

	m_length = sizeof(struct sockaddr_in6);
}

bool SocketAddress::equal(const std::string &ip) const
{
	if (IPAddress::isIPv6(ip) != m_isIPv6)
		return false;
	if (m_isIPv6)
	{
		const std::string		   ipWithoutBrackets = ip.substr(1, ip.size() - 2);
		struct in6_addr			   addr				 = IPAddress::convertIPv6(ipWithoutBrackets);
		const struct sockaddr_in6 *currentAddr		 = reinterpret_cast<const struct sockaddr_in6 *>(&m_storage);
		return std::memcmp(&currentAddr->sin6_addr, &addr, sizeof(in6_addr)) == 0;
	}
	return ip == m_ip;
}


SocketAddress SocketAddress::any(const bool ipv6, const unsigned short port)
{
	SocketAddress addr;

	addr.m_isIPv6 = ipv6;
	if (ipv6)
	{
		struct in6_addr anyAddr = IN6ADDR_ANY_INIT;
		addr.initIPv6(port, anyAddr);
		return addr;
	}
	addr.initIPv4(port, htonl(INADDR_ANY));
	return addr;
}

const struct sockaddr *SocketAddress::getAddr() const
{
	return reinterpret_cast<const struct sockaddr *>(&m_storage);
}

unsigned short SocketAddress::getPort() const
{
	return m_port;
}

socklen_t SocketAddress::getLength() const
{
	return m_length;
}

bool SocketAddress::isIPv6() const
{
	return m_isIPv6;
}



bool SocketAddress::isAnyIPv4() const
{
	if (!m_isIPv6)
	{
		const struct sockaddr_in *addr = reinterpret_cast<const struct sockaddr_in *>(&m_storage);
		return addr->sin_addr.s_addr == htonl(INADDR_ANY);
	}
	return false;
}

bool SocketAddress::isAnyIPv6() const
{
	if (m_isIPv6)
	{
		const struct sockaddr_in6	*addr = reinterpret_cast<const struct sockaddr_in6 *>(&m_storage);
		static const struct in6_addr any  = IN6ADDR_ANY_INIT;

		return std::memcmp(&addr->sin6_addr, &any, sizeof(in6_addr)) == 0;
	}
	return false;
}



bool SocketAddress::operator==(const SocketAddress &other) const
{
	if (m_isIPv6 != other.m_isIPv6)
		return false;
	if (m_isIPv6)
	{
		const sockaddr_in6 *a = reinterpret_cast<const sockaddr_in6 *>(&m_storage);
		const sockaddr_in6 *b = reinterpret_cast<const sockaddr_in6 *>(&other.m_storage);
		return std::memcmp(&a->sin6_addr, &b->sin6_addr, sizeof(in6_addr)) == 0 && a->sin6_port == b->sin6_port;
	}
	const sockaddr_in *a = reinterpret_cast<const sockaddr_in *>(&m_storage);
	const sockaddr_in *b = reinterpret_cast<const sockaddr_in *>(&other.m_storage);
	return a->sin_addr.s_addr == b->sin_addr.s_addr && a->sin_port == b->sin_port;
}

bool SocketAddress::operator!=(const SocketAddress &other) const
{
	return !(*this == other);
}

bool SocketAddress::operator<(const SocketAddress &other) const
{
	if (m_isIPv6 != other.m_isIPv6)
		return m_isIPv6 < other.m_isIPv6;
	if (m_isIPv6)
	{
		const sockaddr_in6 *a	= reinterpret_cast<const sockaddr_in6 *>(&m_storage);
		const sockaddr_in6 *b	= reinterpret_cast<const sockaddr_in6 *>(&other.m_storage);
		int					cmp = std::memcmp(&a->sin6_addr, &b->sin6_addr, sizeof(in6_addr));
		if (cmp != 0)
			return cmp < 0;
		return a->sin6_port < b->sin6_port;
	}
	const sockaddr_in *a = reinterpret_cast<const sockaddr_in *>(&m_storage);
	const sockaddr_in *b = reinterpret_cast<const sockaddr_in *>(&other.m_storage);
	if (a->sin_addr.s_addr != b->sin_addr.s_addr)
		return a->sin_addr.s_addr < b->sin_addr.s_addr;
	return a->sin_port < b->sin_port;
}

#include <ostream>

std::ostream &operator<<(std::ostream &os, const SocketAddress &data)
{
	const struct sockaddr *addr = data.getAddr();

	if (data.isIPv6())
	{
		const struct sockaddr_in6 *addr6 = reinterpret_cast<const struct sockaddr_in6 *>(addr);

		os << "[";
		for (int i = 0; i < 8; ++i)
		{
			uint16_t part = ntohs(addr6->sin6_addr.s6_addr16[i]);

			os << std::hex << std::nouppercase << part;

			if (i < 7)
				os << ":";
		}
		os << "]:";

		os << std::dec << ntohs(addr6->sin6_port);
		return os;
	}
	const struct sockaddr_in *addr4 =
		reinterpret_cast<const struct sockaddr_in *>(addr);

	uint32_t ip = ntohl(addr4->sin_addr.s_addr);

	uint8_t a = (ip >> 24) & 0xFF;
	uint8_t b = (ip >> 16) & 0xFF;
	uint8_t c = (ip >> 8) & 0xFF;
	uint8_t d = ip & 0xFF;

	os << std::dec
	   << static_cast<int>(a) << "."
	   << static_cast<int>(b) << "."
	   << static_cast<int>(c) << "."
	   << static_cast<int>(d)
	   << ":"
	   << ntohs(addr4->sin_port);
	return os;
}