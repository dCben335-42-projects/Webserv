#ifndef LOG_VECTOR_TPP
#define LOG_VECTOR_TPP

#include "./Log/Log.hpp"
#include "Vector.hpp"


template <typename VectorType>
std::string getVectorLogJson(const VectorType &vec)
{
	std::string result = "[\n";
	Log::incrementIndent();
	for (size_t i = 0; i < vec.size(); ++i)
	{
		result += Log::indent();
		result += getLogJson(vec[i]);
		if (i + 1 < vec.size())
			result += ",\n";
	}
	Log::decrementIndent();
	result += "\n" + Log::indent() + "]";
	return result;
}

template <typename VectorType>
static void logVectorJson(const VectorType &vec)
{
	std::cout << Log::indent() << "[\n";
	Log::incrementIndent();
	for (size_t i = 0; i < vec.size(); ++i)
	{
		log(vec[i]);
		if (i + 1 < vec.size())
			std::cout << ",\n";
	}
	Log::decrementIndent();
	std::cout
		<< "\n"
		<< Log::indent() << "]";
}

template <typename V>
void log(const std::vector<V> &vec)
{
	logVectorJson(vec);
}

template <typename V>
void log(const Vec<V> &vec)
{
	logVectorJson(vec);
}

template <typename V>
std::string getLogJson(const Vec<V> &vec)
{
	return getVectorLogJson(vec);
}

template <typename V>
std::string getLogJson(const std::vector<V> &vec)
{
	return getVectorLogJson(vec);
}


#endif
