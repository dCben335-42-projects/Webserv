#include "SocketAddress.hpp"
#include "IPAddress.hpp"
#include <cctype>

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
	m_isIPv6 = IPAddress::isIPv6(ip);
	if (m_isIPv6)
	{
		const std::string ipWithoutBrackets = ip.substr(1, ip.size() - 2);
		initIPv6(ipWithoutBrackets, port);
	}
	else
	{

		initIPv4(ip, port);
	}
}


void SocketAddress::initIPv4(const std::string &ip, unsigned short port)
{
	struct sockaddr_in *addr =
		reinterpret_cast<struct sockaddr_in *>(&m_storage);

	addr->sin_family = AF_INET;
	addr->sin_port	 = htons(port);

	if (std::isalpha(ip[0]))
		addr->sin_addr.s_addr = IPAddress::resolveIPv4(ip);
	else
		addr->sin_addr.s_addr = IPAddress::convertIPv4(ip);

	m_length = sizeof(struct sockaddr_in);
}

void SocketAddress::initIPv6(const std::string &ip, unsigned short port)
{
	struct sockaddr_in6 *addr =
		reinterpret_cast<struct sockaddr_in6 *>(&m_storage);

	addr->sin6_family = AF_INET6;
	addr->sin6_port	  = htons(port);

	if (std::isalpha(ip[0]))
		addr->sin6_addr = IPAddress::resolveIPv6(ip);
	else
		addr->sin6_addr = IPAddress::convertIPv6(ip);

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

#include "Logger.hpp"

bool SocketAddress::operator==(const SocketAddress &other) const
{
	Logger::error("Comparing SocketAddress: " + Logger::get(*this) + " with " + Logger::get(other));
	if (m_isIPv6 != other.m_isIPv6)
		return false;

	if (m_isIPv6)
	{
		const sockaddr_in6 *a = reinterpret_cast<const sockaddr_in6 *>(&m_storage);
		const sockaddr_in6 *b = reinterpret_cast<const sockaddr_in6 *>(&other.m_storage);
		return std::memcmp(&a->sin6_addr, &b->sin6_addr, sizeof(in6_addr)) == 0 && a->sin6_port == b->sin6_port;
	}
	else
	{
		const sockaddr_in *a = reinterpret_cast<const sockaddr_in *>(&m_storage);
		const sockaddr_in *b = reinterpret_cast<const sockaddr_in *>(&other.m_storage);
		return a->sin_addr.s_addr == b->sin_addr.s_addr && a->sin_port == b->sin_port;
	}
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
		char ip[INET6_ADDRSTRLEN];
		inet_ntop(AF_INET6, &addr6->sin6_addr, ip, sizeof(ip));
		os << ip << ":" << ntohs(addr6->sin6_port);
	}
	else
	{
		const struct sockaddr_in *addr4 =
			reinterpret_cast<const struct sockaddr_in *>(addr);
		char ip[INET_ADDRSTRLEN];
		inet_ntop(AF_INET, &addr4->sin_addr, ip, sizeof(ip));
		os << ip << ":" << ntohs(addr4->sin_port);
	}

	return os;
}