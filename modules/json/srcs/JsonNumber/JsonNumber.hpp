#ifndef KIT_JSON_NUMBER_HPP
#define KIT_JSON_NUMBER_HPP

#include "../AJson.hpp"

class JsonNumber : public AJson
{
	private:
		double m_value;

	public:
		JsonNumber(const double value);
		double get() const;
};

std::ostream &operator<<(std::ostream &os, const JsonNumber &num);

#endif