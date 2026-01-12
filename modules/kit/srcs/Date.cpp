#include "../includes/Date.hpp"
#include <ctime>
#include <stdexcept>

std::string Date::getCurrentDate()
{
	time_t now = time(0);

	return Date::toHttpDateFormat(gmtime(&now));
}

std::string Date::toHttpDateFormat(const tm *timeInfo)
{
	if (!timeInfo)
		return "";
	char buf[100];

	strftime(buf, sizeof(buf), HTTP_DATE_FORMAT, timeInfo);
	return std::string(buf);
}

bool Date::compareHttpDates(const std::string &date1, const std::string &date2)
{
	struct tm tstruct1;
	struct tm tstruct2;

	strptime(date1.c_str(), HTTP_DATE_FORMAT, &tstruct1);
	strptime(date2.c_str(), HTTP_DATE_FORMAT, &tstruct2);

	time_t time1 = mktime(&tstruct1);
	time_t time2 = mktime(&tstruct2);

	return difftime(time1, time2) > 0;
}