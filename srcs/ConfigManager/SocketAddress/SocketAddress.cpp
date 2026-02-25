#include "SocketAddress.hpp"
#include "IPAddress.hpp"
#include <cctype>
#include "Logger.hpp"


SocketAddress::SocketAddress()
{
	std::memset(&m_storage, 0, sizeof(m_storage));
	m_length = 0;
	m_isIPv6 = false;
}

SocketAddress::SocketAddress(const std::string &ip, unsigned short port)
{
	initIP(ip, port);
}

void SocketAddress::initIP(const std::string &ip, unsigned short port)
{
	std::memset(&m_storage, 0, sizeof(m_storage));

	const bool isHostname = IPAddress::isHostname(ip);

	if (isHostname)
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
	}
	else
	{
		initIPv4(port, IPAddress::convertIPv4(ip));
	}
}


void SocketAddress::initIPv4(unsigned short port, uint32_t sin_addr)
{
	struct sockaddr_in *addr = reinterpret_cast<struct sockaddr_in *>(&m_storage);

	addr->sin_family	  = AF_INET;
	addr->sin_port		  = htons(port);
	addr->sin_addr.s_addr = sin_addr;
	m_length			  = sizeof(struct sockaddr_in);
}


void SocketAddress::initIPv6(unsigned short port, in6_addr sin6_addr)
{
	struct sockaddr_in6 *addr = reinterpret_cast<struct sockaddr_in6 *>(&m_storage);

	addr->sin6_family = AF_INET6;
	addr->sin6_port	  = htons(port);
	addr->sin6_addr	  = sin6_addr;

	m_length = sizeof(struct sockaddr_in6);
}

const struct sockaddr *SocketAddress::getAddr() const
{
	return reinterpret_cast<const struct sockaddr *>(&m_storage);
}

socklen_t SocketAddress::getLength() const
{
	return m_length;
}

bool SocketAddress::isIPv6() const
{
	return m_isIPv6;
}


bool SocketAddress::operator==(const SocketAddress &other) const
{
	Logger::debug("Comparing SocketAddress: " + Logger::get(*this) + " with " + Logger::get(other));
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

#include <ostream>

std::ostream &operator<<(std::ostream &os, const SocketAddress &data)
{
	const struct sockaddr *addr = data.getAddr();

	if (data.isIPv6())
	{
		const struct sockaddr_in6 *addr6 =
			reinterpret_cast<const struct sockaddr_in6 *>(addr);

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