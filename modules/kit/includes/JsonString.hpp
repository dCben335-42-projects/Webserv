#ifndef JSONSTRING_HPP
#define JSONSTRING_HPP

#include <string>

class JsonString
{
	private:
		std::string m_value;

	public:
		JsonString();
		explicit JsonString(const std::string &val);
		JsonString(const JsonString &other);

		JsonString &operator=(const JsonString &other);

		const std::string &getValue() const;
		void			   setValue(const std::string &val);

		std::string toString() const;
};

#endif
