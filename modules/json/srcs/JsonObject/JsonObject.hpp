#ifndef KIT_JSON_OBJECT_HPP
#define KIT_JSON_OBJECT_HPP

#include "../AJson.hpp"

class JsonObject : public AJson
{
	protected:
		Map<std::string, AJson *> m_members;

	public:
		JsonObject();
		JsonObject(const Map<std::string, AJson *> &members);
		~JsonObject();

		const Map<std::string, AJson *> &get() const;
		const AJson						*get(const std::string &key) const;

		void set(const std::string &key, AJson *value);
		void remove(const std::string &key);

		void create(const std::string &key, const std::string &value);
		void create(const std::string &key, const double value);
		void create(const std::string &key, const bool value);
		void create(const std::string &key, const Vec<AJson *> &array);
		void create(const std::string &key, const Map<std::string, AJson *> &object);
};

std::ostream &operator<<(std::ostream &os, const JsonObject &obj);

#endif