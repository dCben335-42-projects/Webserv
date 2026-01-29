#include <ostream>
#include "Accept.hpp"
#include "String.hpp"

Accept::Accept(const std::string &entry)
{
	size_t semicolonPos = entry.find(';');
	if (semicolonPos != std::string::npos)
	{
		m_mediaType			  = String::trim(entry.substr(0, semicolonPos));
		std::string qValueStr = String::trim(entry.substr(semicolonPos + 1));
		if (qValueStr.substr(0, 2) == "q=")
		{
			m_qValue = static_cast<float>(std::atof(qValueStr.substr(2).c_str()));
		}
		else
		{
			m_qValue = 1.0f;
		}
	}
	else
	{
		m_mediaType = String::trim(entry);
		m_qValue	= 1.0f;
	}
}

Accept::~Accept()
{
}


const std::string &Accept::getMediaType() const
{
	return m_mediaType;
}

float Accept::getQValue() const
{
	return m_qValue;
}




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