#include "IPAddress.hpp"
#include "Number.hpp"


/**
 *
 * @brief Determines if the given IP address string is an IPv6 address.
 *
 * @param ip The IP address string to check.
 * @return true if the IP address is IPv6, false otherwise.
 *
 * @note This function checks for the presence of a colon (':') in the IP address string,
 * which is a characteristic of IPv6 addresses. If a colon is found, it returns true,
 * indicating that the IP address is likely an IPv6 address. If no colon is found, it returns false,
 * indicating that the IP address is likely an IPv4 address.
 *
 */
bool IPAddress::isIPv6(const std::string &ip)
{
	return ip.find(':') != std::string::npos;
}



/**
 *
 * @brief Builds a struct in6_addr from an array of 8 16-bit parts representing an IPv6 address.
 *
 * @param parts An array of 8 16-bit integers representing the parts of the IPv6 address.
 * @return The constructed IPv6 address as a struct in6_addr.
 *
 * @note This function takes an array of 8 16-bit integers, which represent the 8 segments of an IPv6 address,
 * and constructs a struct in6_addr by converting each part to network byte order using htons and storing it
 * in the s6_addr16 field of the struct.
 *
 */
struct in6_addr IPAddress::buildIPv6(const uint16_t (&parts)[8])
{
	struct in6_addr addr;
	std::memset(&addr, 0, sizeof(addr));

	for (int i = 0; i < 8; ++i)
		addr.s6_addr16[i] = htons(parts[i]);

	return addr;
}


/**
 *
 * @brief Expands a compressed IPv6 address by filling in the missing parts with zeros.
 *
 * @param parts An array of 8 16-bit integers representing the parts of the IPv6 address, where some parts may be missing due to compression.
 * @param partIndex The current index in the parts array where the next part will be added.
 * @param compressIndex The index in the parts array where the compression occurred (i.e., where "::" was found).
 *
 * @note This function calculates how many parts are missing due to compression and shifts the existing parts to make room for the missing parts.
 * It then fills the missing parts with zeros. The function modifies the input array in place to represent the expanded IPv6 address.
 *
 */
void IPAddress::expandCompressed(
	uint16_t (&parts)[8],
	const int partIndex,
	const int compressIndex)
{
	int missing = 8 - partIndex;

	for (int i = partIndex - 1; i >= compressIndex; --i)
		parts[i + missing] = parts[i];

	for (int i = compressIndex; i < compressIndex + missing; ++i)
		parts[i] = 0;
}


/**
 *
 * @brief Parses the parts of an IPv6 address from a string representation and fills an array with the parsed values.
 *
 * @param ip The string representation of the IPv6 address (e.g., "2001:0db8:85a3::8a2e:0370:7334").
 * @param parts An array of 8 16-bit integers to be filled with the parsed parts of the IPv6 address.
 * @param partIndex A reference to an integer that will be updated with the current index in the parts array where the next part will be added.
 * @param compressIndex A reference to an integer that will be updated with the index in the parts array where compression occurred (i.e., where "::" was found), or -1 if no compression is present.
 *
 * @note This function iterates through the characters of the input string, parsing hexadecimal values for each part of the IPv6 address.
 * It handles both normal parts separated by colons and compressed parts indicated by "::". The parsed values are stored in the provided parts array,
 * and the indices for part addition and compression are updated accordingly.
 *
 */
void IPAddress::parseIPv6Parts(
	const std::string &ip,
	uint16_t (&parts)[8],
	int &partIndex,
	int &compressIndex)
{
	uint16_t value = 0;

	for (size_t i = 0; i <= ip.length(); ++i)
	{
		if (ip[i] == ':')
		{
			if (i > 0 && ip[i - 1] == ':')
			{
				compressIndex = partIndex;
				continue;
			}

			parts[partIndex++] = value;
			value			   = 0;
		}
		else if (ip[i] == '\0')
		{
			parts[partIndex++] = value;
		}
		else
		{
			value = (value << 4) | Number::hexToInt(ip[i]);
		}
	}
}

/**
 *
 * @brief Converts a string representation of an IPv6 address to a struct in6_addr.
 *
 * @param ip The string representation of the IPv6 address (e.g., "2001:0db8:85a3:0000:0000:8a2e:0370:7334").
 * @return The IPv6 address as a struct in6_addr.
 *
 * @warning This function assumes the input string is a valid IPv6 address and does not perform error checking.
 *
 */
struct in6_addr IPAddress::convertIPv6(const std::string &ip)
{
	uint16_t parts[8]	   = {0};
	int		 partIndex	   = 0;
	int		 compressIndex = -1;

	parseIPv6Parts(ip, parts, partIndex, compressIndex);
	if (compressIndex != -1)
		expandCompressed(parts, partIndex, compressIndex);
	return buildIPv6(parts);
}


/**
 *
 * @brief Resolves a hostname to an IPv6 address and returns it as a struct in6_addr.
 *
 * @param hostname The hostname to resolve (e.g., "example.com").
 * @return The resolved IPv6 address as a struct in6_addr.
 * @note This function uses the getaddrinfo system call to resolve the hostname to an IPv6 address.
 * It sets up the appropriate hints for IPv6 and SOCK_STREAM, and retrieves the first result.
 *
 * @throws std::runtime_error if the hostname cannot be resolved or if getaddrinfo fails.
 *
 */
struct in6_addr IPAddress::resolveIPv6(const std::string &hostname)
{
	struct addrinfo	 hints;
	struct addrinfo *result;

	std::memset(&hints, 0, sizeof(hints));
	hints.ai_family	  = AF_INET6;
	hints.ai_socktype = SOCK_STREAM;

	if (getaddrinfo(hostname.c_str(), NULL, &hints, &result) != 0)
		throw std::runtime_error("Failed to resolve IPv6 address");

	struct sockaddr_in6 *addr =
		reinterpret_cast<struct sockaddr_in6 *>(result->ai_addr);

	struct in6_addr ip = addr->sin6_addr;

	freeaddrinfo(result);
	return ip;
}


/**
 *
 * @brief Converts a string representation of an IPv4 address to a 32-bit integer in network byte order.
 *
 * @param ip The string representation of the IPv4 address (e.g., "192.168.0.1").
 * @return The IPv4 address as a 32-bit integer in network byte order.
 *
 */
uint32_t IPAddress::convertIPv4(const std::string &ip)
{
	uint32_t addr  = 0;
	uint32_t value = 0;
	int		 shift = 24;

	for (size_t i = 0; i <= ip.length(); ++i)
	{
		if (ip[i] == '.' || ip[i] == '\0')
		{
			addr |= (value << shift);
			value = 0;
			shift -= 8;
		}
		else
		{
			value = value * 10 + (ip[i] - '0');
		}
	}

	return htonl(addr);
}


/**
 *
 * @brief Resolves a hostname to an IPv4 address and returns it as a 32-bit integer in network byte order.
 *
 * @param hostname The hostname to resolve (e.g., "example.com").
 * @return The resolved IPv4 address as a 32-bit integer in network byte order.
 * @note This function uses the getaddrinfo system call to resolve the hostname to an IPv4 address.
 * It sets up the appropriate hints for IPv4 and SOCK_STREAM, and retrieves the first result.
 *
 * @throws std::runtime_error if the hostname cannot be resolved or if getaddrinfo fails.
 *
 */
uint32_t IPAddress::resolveIPv4(const std::string &hostname)
{
	struct addrinfo	 hints;
	struct addrinfo *result;

	std::memset(&hints, 0, sizeof(hints));
	hints.ai_family	  = AF_INET;
	hints.ai_socktype = SOCK_STREAM;

	if (getaddrinfo(hostname.c_str(), NULL, &hints, &result) != 0)
		throw std::runtime_error("Failed to resolve IPv4 address");

	struct sockaddr_in *addr =
		reinterpret_cast<struct sockaddr_in *>(result->ai_addr);

	uint32_t ip = addr->sin_addr.s_addr;

	freeaddrinfo(result);
	return ip;
}