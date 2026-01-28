#ifndef JSONOBJECT_HPP
#define JSONOBJECT_HPP

#include <map>
#include <string>
#include "Json_fwd.hpp"

class JsonObject
{
	private:
		std::map<std::string, Json *> m_members;

	public:
		JsonObject();
		JsonObject(const JsonObject &other);
		~JsonObject();

		JsonObject &operator=(const JsonObject &other);

		void		insert(const std::string &key, Json *value);
		Json	   *get(const std::string &key);
		const Json *get(const std::string &key) const;
		bool		has(const std::string &key) const;
		void		remove(const std::string &key);
		void		clear();
		size_t		size() const;

		std::string toString() const;

		typedef std::map<std::string, Json *>::iterator		  iterator;
		typedef std::map<std::string, Json *>::const_iterator const_iterator;

		iterator	   begin();
		iterator	   end();
		const_iterator begin() const;
		const_iterator end() const;
};

#endif