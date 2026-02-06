#include <ostream>
#include "Accept.hpp"
#include "String.hpp"

#define ACCEPT_Q_VALUE_PREFIX "q="
#define ACCEPT_Q_VALUE_PREFIX_LEN 2

Accept::Accept(const std::string &entry)
{
	const size_t semicolonPos = entry.find(';');
	if (semicolonPos == std::string::npos)
	{
		m_mediaType = String::trim(entry);
		m_qValue	= 1.0f;
		return;
	}
	const std::string qValueStr	   = String::trim(entry.substr(semicolonPos + 1));
	const std::string qValuePrefix = String::trim(qValueStr.substr(0, ACCEPT_Q_VALUE_PREFIX_LEN));

	m_mediaType = String::trim(entry.substr(0, semicolonPos));
	m_qValue	= qValuePrefix == ACCEPT_Q_VALUE_PREFIX
					  ? static_cast<float>(std::atof(qValueStr.c_str()))
					  : 1.0f;
}

Accept::~Accept()
{
}


/**
 *
 * @brief Gets the media type of the Accept header entry.
 *
 * @return A string containing the media type of the Accept header entry.
 *
 * */
const std::string &Accept::getMediaType() const
{
	return m_mediaType;
}

/**
 *
 * @brief Gets the q-value of the Accept header entry.
 *
 * @return A float containing the q-value of the Accept header entry.
 *
 * */
float Accept::getQValue() const
{
	return m_qValue;
}


/**
 *
 * @brief Compares two Accept objects for sorting.
 *
 * The comparison is based on the q-value, with higher q-values considered "less" than lower ones (for descending order).
 * If the q-values are equal, the media types are compared lexicographically.
 *
 * @param a The first Accept object to compare.
 * @param b The second Accept object to compare.
 *
 * @note It is meant to be used as a comparison function for sorting a collection of Accept objects,
 * such as in std::sort.
 *
 * @return true if a should come before b in the sorted order, false otherwise.
 *
 * */
bool Accept::compare(const Accept &a, const Accept &b)
{
	if (a.m_qValue == b.m_qValue)
		return a.m_mediaType < b.m_mediaType;
	return a.m_qValue > b.m_qValue;
}



std::ostream &operator<<(std::ostream &o, const Accept &accept)
{
	o << "{ \"mediaType\": \"" << accept.getMediaType() << "\", \"qValue\": " << accept.getQValue() << " }";
	return o;
}