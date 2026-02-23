#ifndef KIT_JSON_ARRAY_HPP
#define KIT_JSON_ARRAY_HPP

#include "../AJson.hpp"

class JsonArray : public AJson
{
	private:
		Vec<AJson *> m_elements;

	public:
		JsonArray();
		JsonArray(const Vec<AJson *> &elements);
		~JsonArray();

		typedef Vec<AJson *>::iterator iterator;

		const Vec<AJson *> &get() const;
		AJson			   *get(size_t index) const;
		AJson			   *getLast() const;

		bool   has(size_t index) const;
		void   push(AJson *value);
		void   remove(size_t index);
		bool   contains(AJson *value) const;
		size_t size() const;

		void create(const char *value);
		void create(const std::string &value);
		void create(double value);
		void create(bool value);
		void create(const Vec<AJson *> &array);
		void create(const Map<std::string, AJson *> &object);
};

std::ostream &operator<<(std::ostream &os, const JsonArray &arr);

#endif