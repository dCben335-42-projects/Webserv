#ifndef JSON_HPP
#define JSON_HPP

#include <string>
#include "JsonString.hpp"
#include "JsonVector.hpp"
#include "JsonObject.hpp"

class Json
{
	public:
		enum Type
		{
			TYPE_NULL,
			TYPE_BOOL,
			TYPE_NUMBER,
			TYPE_STRING,
			TYPE_ARRAY,
			TYPE_OBJECT
		};

	private:
		Type type_;

		bool		m_bool;
		double		m_number;
		JsonString *m_str;
		JsonVector *m_vec;
		JsonObject *m_obj;

		void cleanup();
		void copy(const Json &other);

	public:
		Json();
		explicit Json(Type type);
		explicit Json(bool value);
		explicit Json(int value);
		explicit Json(double value);
		explicit Json(const std::string &value);
		explicit Json(const char *value);
		Json(const Json &other);
		~Json();

		Json &operator=(const Json &other);

		Type getType() const;
		bool isNull() const;
		bool isBool() const;
		bool isNumber() const;
		bool isString() const;
		bool isArray() const;
		bool isObject() const;

		bool			  asBool() const;
		double			  asNumber() const;
		int				  asInt() const;
		std::string		  asString() const;
		JsonVector		 &asArray();
		const JsonVector &asArray() const;
		JsonObject		 &asObject();
		const JsonObject &asObject() const;

		std::string toString() const;

		static Json *createNull();
		static Json *createBool(bool value);
		static Json *createNumber(double value);
		static Json *createString(const std::string &value);
		static Json *createArray();
		static Json *createObject();
};

#endif
