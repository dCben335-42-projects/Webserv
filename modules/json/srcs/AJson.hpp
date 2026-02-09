#ifndef KIT_A_JSON_HPP
#define KIT_A_JSON_HPP

#include <string>
#include <Vec.hpp>
#include <Map.hpp>

class JsonArray;
class JsonObject;
class JsonString;
class JsonNumber;
class JsonBool;
class JsonNull;

class AJson
{
	public:
		enum Type
		{
			STRING,
			NUMBER,
			BOOL,
			ARRAY,
			OBJECT,
			NIL
		};

	protected:
		Type m_type;
		AJson(const Type type);


	public:
		virtual ~AJson();
		Type getType() const;

		JsonArray		*asArray();
		const JsonArray *asArray() const;

		JsonObject		 *asObject();
		const JsonObject *asObject() const;

		JsonString		 *asString();
		const JsonString *asString() const;

		JsonNumber		 *asNumber();
		const JsonNumber *asNumber() const;

		JsonBool	   *asBool();
		const JsonBool *asBool() const;

		JsonNull	   *asNull();
		const JsonNull *asNull() const;

		static JsonArray  *createArray();
		static JsonArray  *createArray(const Vec<AJson *> &elements);
		static JsonObject *createObject();
		static JsonObject *createObject(const Map<std::string, AJson *> &members);
		static JsonString *createString(const std::string &value);
		static JsonNumber *createNumber(double value);
		static JsonBool	  *createBool(bool value);
		static JsonNull	  *createNull();
};

std::ostream &operator<<(std::ostream &os, const AJson &json);



#endif