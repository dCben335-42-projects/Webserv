#ifndef SOCKET_ADDRESS_HPP
#define SOCKET_ADDRESS_HPP

#include <string>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

class SocketAddress
{
	private:
		struct sockaddr_storage m_storage;
		socklen_t				m_length;
		bool					m_isIPv6;
		unsigned short			m_port;
		std::string				m_ip;
		bool					m_isHostname;

		void initIPv4(unsigned short port, uint32_t sin_addr);
		void initIPv6(unsigned short port, in6_addr sin6_addr);

	public:
		SocketAddress();
		SocketAddress(const SocketAddress &other);
		SocketAddress(const std::string &ip, uint16_t port);

		void initIP(const std::string &ip, uint16_t port);


		const struct sockaddr *getAddr() const;
		unsigned short		   getPort() const;
		socklen_t			   getLength() const;
		bool				   isIPv6() const;

		bool isAnyIPv4() const;
		bool isAnyIPv6() const;

		bool operator==(const SocketAddress &other) const;
		bool operator!=(const SocketAddress &other) const;
		bool operator<(const SocketAddress &other) const;

		bool equal(const std::string &ip) const;

		static SocketAddress any(bool ipv6, unsigned short port);
};

std::ostream &operator<<(std::ostream &os, const SocketAddress &data);

#endif
