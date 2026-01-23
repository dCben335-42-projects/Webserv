#ifndef KIT_NUMBER_HPP
#define KIT_NUMBER_HPP

#include <string>

class Number
{
	private:
		Number();
		~Number();

	public:
		static std::string toString(const int number);
		static int		   fromString(const std::string &str);
		static bool		   isNumber(const std::string &str);
		static size_t	   toSizeT(const std::string &str);
		static int		   toInt(const std::string &str);
		static std::string bytesToHumanSize(const size_t bytes);
};

#endif
