#ifndef KIT_DATE_HPP
#define KIT_DATE_HPP

#include <string>
#include <ctime>

#define KIT_HTTP_DATE_FORMAT "%a, %d %b %Y %H:%M:%S GMT"
#define KIT_HUMAN_DATE_FORMAT "%Y-%m-%d %H:%M"

class Date
{
	public:
		static std::string getCurrentDate();
		static std::string toHttpDateFormat(const tm *timeInfo);
		static bool		   compareHttpDates(const std::string &date1, const std::string &date2);
		static std::string toHumanFormat(const tm *timeInfo);
};

#endif
