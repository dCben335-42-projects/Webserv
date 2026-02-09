#include "DataCollector.hpp"

DataCollector::DataCollector()
{
}

DataCollector::~DataCollector()
{
}

bool same_prec(ARulePath path, ARulePath pathNext, size_t n)
{
	if (path.size() < n || pathNext.size() < n)
		return false;

	for (size_t i = 0; i < n; ++i)
	{
		if (path[i] != pathNext[i])
			return false;
	}
	return true;
}


void DataCollector::recurseJson(
	ARulePath				path,
	Vec<ARulePath>			pathNexts,
	size_t					index_rule,
	AJson				  **json,
	size_t				   *index,
	Vec<std::string>	   &line,
	std::string			   &buffer,
	Map<size_t, AJson *>   &mapIndexToJson,
	Vec<const ARule *>	   &collectStr,
	bool					hasCollectStr,
	bool				   &continueBuffer,
	Map<size_t, ARulePath> &pathStack)
{

	AJson		*ptr  = NULL;
	const ARule *rule = path[index_rule];


	Logger::log("parent_cpt: " + LoggerJson::get(rule->getConceptName()));
	// Logger::log("parent_cpt: " + LoggerJson::get(rule));
	Logger::log("hasCollectStr: " + LoggerJson::get(hasCollectStr));

	bool tempHasCollectStr = hasCollectStr;
	(void) tempHasCollectStr;



	if (!hasCollectStr && rule->getConceptPtr()->isTerminalConcept())
	{
		collectStr	  = path.slice(index_rule);
		hasCollectStr = true;
		Logger::log("Ok8888");
	}

	if (path.size() - 1 == index_rule)
	{
		size_t lineSize = line[0].size();
		buffer.append(line[0]);
		line.remove(0);
		(*index) += lineSize;
		Logger::log(buffer);

		if (*index < pathNexts.size() && ((hasCollectStr && !path[index_rule - 1]->isSequence() && same_prec(path, pathNexts[*index], index_rule)) || same_prec(path, pathNexts[*index], index_rule + 1)))
		{
			continueBuffer = true;
			Logger::log("Ok333");
		}
		if (*index < pathNexts.size() && !same_prec(path, pathNexts[*index], index_rule))
		{
			Logger::log("Ok4");
			if (!hasCollectStr)
			{
				Logger::log("Ok18");

				*json = AJson::createString(buffer);
				buffer.clear();
			}
		}
		if (index_rule == 0)
		{
			Logger::log("Ok555");
			*json = AJson::createString(buffer);
			buffer.clear();
		}
		return;
	}

	Logger::log("OkSKIP");


	recurseJson(path, pathNexts, index_rule + 1, &ptr, index, line, buffer, mapIndexToJson, collectStr, hasCollectStr, continueBuffer, pathStack);
	Logger::log("----<UP>----");

	if (!ptr)
	{

		if (hasCollectStr && !continueBuffer)
		{
			if (*index < pathNexts.size() && same_prec(path, pathNexts[*index], index_rule))
			{
				continueBuffer = true;
			}
		}

		if (!continueBuffer && !hasCollectStr)
		{
			Logger::log("->1");
			if (index_rule + 1 < path.size())
			{
				Logger::log("->2");
				Logger::log("concept: " + LoggerJson::get(path[index_rule + 1]->getConceptName()));

				if (!path[index_rule + 1]->getConceptPtr()->isTerminalConcept())
				{
					Logger::log("->3");

					if (pathStack.has(index_rule + 1))
					{
						if (pathStack.get(index_rule + 1) != path.slice(index_rule + 1))
						{
							Logger::log("get");
							AJson *currentJson2 = mapIndexToJson.get(index_rule + 1);
							mapIndexToJson.get(index_rule)->asArray()->push(currentJson2);
							mapIndexToJson.remove(index_rule + 1);
						}
					}

					const std::string key = path[index_rule + 1]->getConceptName();
					if (!mapIndexToJson.has(index_rule + 1))
					{
						Logger::log("create_object");

						pathStack.set(index_rule + 1, path.slice(index_rule + 1));
						mapIndexToJson.set(index_rule + 1, AJson::createObject());
						mapIndexToJson.get(index_rule + 1)->asObject()->set(key, AJson::createArray());

						Logger::log("Object created: " + LoggerJson::get(mapIndexToJson.get(index_rule + 1)));
					}

					if (!buffer.empty())
					{
						if (path[index_rule + 1]->isSequence())
						{
							AJson	  *currentJson	 = mapIndexToJson.get(index_rule + 1);
							JsonArray *currentObject = currentJson->asObject()->get(path[index_rule + 1]->getConceptName())->asArray();
							AJson	  *lastElement	 = currentObject->getLast();
							if (!lastElement || lastElement->getType() != AJson::ARRAY)
								currentObject->push(AJson::createArray());
							currentObject->getLast()->asArray()->push(AJson::createString(buffer));
						}
						else
						{
							mapIndexToJson.get(index_rule + 1)->asObject()->get(key)->asArray()->push(AJson::createString(buffer));

							if (path[index_rule]->isSequence() && path[index_rule + 1]->isEnd())
							{
								const std::string key2			= path[index_rule]->getConceptName();
								JsonArray		 *currentObject = mapIndexToJson.get(index_rule)->asObject()->get(key2)->asArray();

								AJson *lastElement = currentObject->getLast();
								if (!lastElement || lastElement->getType() != AJson::ARRAY)
								{
									currentObject->push(AJson::createArray());
									lastElement = currentObject->getLast()->asArray();
								}
								lastElement->asArray()->push(mapIndexToJson.get(index_rule + 1));
								mapIndexToJson.remove(index_rule + 1);
								pathStack.remove(index_rule + 1);
							}
						}
						buffer.clear();
					}

					if (*index >= pathNexts.size() || (!same_prec(path, pathNexts[*index], index_rule + 1)))
					{
						Logger::log("Ok56");
						AJson *currentJson2 = mapIndexToJson.get(index_rule + 1);
						if (index_rule == 0)
							mapIndexToJson.get(index_rule)->asArray()->push(currentJson2);
						else
						{
							mapIndexToJson.get(index_rule)->asObject()->get(path[index_rule]->getConceptName())->asArray()->push(currentJson2);
						}
						Logger::log("Ok50");
						mapIndexToJson.remove(index_rule + 1);
						pathStack.remove(index_rule + 1);
					}

					Logger::log("Ok97");


					return;
				}
			}
		}

		// if (!hasCollectStr && !rule->getConceptPtr()->isTerminalConcept())
		// {
		// 	*json = AJson::createObject();
		// }

		if (!continueBuffer && index_rule == 0)
		{
			Logger::log("Ok99995");
			*json = AJson::createString(buffer);
			return;
		}
		Logger::log("Ok59");

		return;
	}
	else
	{
		Logger::log("Ok6");

		Logger::log("string : " + LoggerJson::get(buffer));
		// Si TerminalConcept on doit ajouter des string au create string
		if (path[index_rule]->isSequence())
		{
			if (!mapIndexToJson.has(index_rule))
				mapIndexToJson.set(index_rule, AJson::createArray());
		}
		else
		{
			if (!mapIndexToJson.has(index_rule))
				mapIndexToJson.set(index_rule, AJson::createArray());
		}

		AJson *currentJson = mapIndexToJson.get(index_rule);
		if (currentJson->getType() == AJson::ARRAY)
		{
			currentJson->asArray()->push(ptr);
		}
		if (!same_prec(path, pathNexts[*index], index_rule - 1))
		{
			*json = currentJson;
			return;
		}
	}
}

void DataCollector::generateJson(const BNFPath *cursor, const std::string &stringToParsed)
{
	// clang-format off

	// START VARS

	// ARulePath path = cursor->rulePath;
	// JsonArray *line	 = AJson::createArray();
	// size_t index = 0;
	// AJson *ptr = NULL;

	Vec<ARulePath> pathStack;
	pathStack.push_back(cursor->rulePath);

	size_t inc = 0;
	while (cursor && cursor->next.size() > 0)
	{
		for (size_t i = 0; i < cursor->next.size(); ++i)
		{
			if (!cursor->next[i].isInvalidedPath)
			{
				cursor = &cursor->next[i];
				inc++;
				break;
			}
		}
		pathStack.push_back(cursor->rulePath);
	}

	// [....][.....]
	// [..][..][...]
	// Logger::log(LoggerJson::get(pathStack, true));

	Vec<std::string> line;
	std::string str = "";

	if (!pathStack.empty())
	{
		str.push_back(stringToParsed[0]);  // char

		for (size_t i = 1; i < pathStack.size(); ++i)
		{
			if (pathStack[i - 1] == pathStack[i])
			{
				str.push_back(stringToParsed[i]);  // char
			}
			else
			{
				line.push_back(str);
				Logger::log(LoggerJson::get(str));
				str = "";
				str.push_back(stringToParsed[i]);
			}
		}

		line.push_back(str);
	}

	Vec<AJson *> elements;
	AJson *json = NULL;
	std::string buffer;
	size_t index = 0;


	Map<size_t, AJson *> mapIndexToJson;
	bool continueBuffer = false;
	Vec<const ARule *> collectStr;

	Map<size_t, ARulePath> pathsAll;

	AJson *arr = AJson::createArray();
	mapIndexToJson.set(0, arr);

	while (index < pathStack.size())
	{
		Logger::log("=======");
		recurseJson(pathStack[index], pathStack, 0, &json, &index, line, buffer, mapIndexToJson, collectStr, false, continueBuffer, pathsAll);
		Logger::log("json : " + LoggerJson::get(json));
		if (!continueBuffer)
		{
			buffer.clear();
		}
		continueBuffer = false;
		if (json)
		{
			mapIndexToJson.get(0)->asArray()->push(json);
			json = NULL;
		}
		if (mapIndexToJson.has(0))
			Logger::log("arr : " + LoggerJson::get(mapIndexToJson.get(0), false));
	}
	// Logger::log(line2);
}

