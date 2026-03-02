#ifndef CONFIG_LOCATION_RETURN_HPP
#define CONFIG_LOCATION_RETURN_HPP

#include <string>

class LocationReturn
{
	private:
		std::string	   m_target;
		unsigned short m_statusCode;

	public:
		LocationReturn();
		LocationReturn(const int statusCode, const std::string &target = "");
		~LocationReturn();

		const std::string &getTarget() const;
		unsigned short	   getStatusCode() const;

		void setTarget(const std::string &target);
		void setStatusCode(const int statusCode);
};

std::ostream &operator<<(std::ostream &os, const LocationReturn &locationReturn);

#endif