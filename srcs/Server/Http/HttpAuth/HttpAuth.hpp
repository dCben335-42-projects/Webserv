#ifndef HTTPAUTH_HPP
#define HTTPAUTH_HPP

#include "Map.hpp"

class HttpAuth
{
	private:
		Map<std::string> m_urlEncoded;

	public:
		HttpAuth();
		~HttpAuth();

		void parse(const std::string &body);
};

#endif