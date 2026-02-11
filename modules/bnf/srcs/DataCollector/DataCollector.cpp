#include "DataCollector.hpp"

DataCollector::DataCollector()
{
}

DataCollector::~DataCollector()
{
}

bool DataCollector::same_prec(const ARulePath &path, const ARulePath &pathNext, const size_t n) const
{
	if (path.size() < n || pathNext.size() < n)
		return false;

	for (size_t i = 0; i < n; ++i)
		if (path[i] != pathNext[i])
			return false;
	return true;
}


void DataCollector::recurseJson(
	size_t	index_rule,
	AJson **json,
	bool	hasCollectStr)
{

	AJson		*ptr  = NULL;
	const ARule *rule = m_path[index_rule];

	if (!hasCollectStr && rule->getConceptPtr()->isTerminalConcept())
	{
		m_collectStr  = m_path.slice(index_rule);
		hasCollectStr = true;
	}

	if (m_path.size() - 1 == index_rule)
	{
		size_t lineSize = m_lines[0].size();
		m_buffer.append(m_lines[0]);
		m_lines.remove(0);
		m_cidx += lineSize;
		Logger::log(m_buffer);

		if (m_cidx < m_pathNexts.size() && ((hasCollectStr && !m_path[index_rule - 1]->isSequence() && same_prec(m_path, m_pathNexts[m_cidx], index_rule)) || same_prec(m_path, m_pathNexts[m_cidx], index_rule + 1)))
		{
			m_continueBuffer = true;
		}
		if (m_cidx < m_pathNexts.size() && !same_prec(m_path, m_pathNexts[m_cidx], index_rule))
		{
			if (!hasCollectStr)
			{
				*json = AJson::createString(m_buffer);
				m_buffer.clear();
			}
		}
		if (index_rule == 0)
		{
			*json = AJson::createString(m_buffer);
			m_buffer.clear();
		}
		return;
	}
	recurseJson(index_rule + 1, &ptr, hasCollectStr);
	Logger::log("----<UP>----");

	if (!ptr)
	{

		if (hasCollectStr && !m_continueBuffer)
		{
			if (m_cidx < m_pathNexts.size() && same_prec(m_path, m_pathNexts[m_cidx], index_rule))
			{
				m_continueBuffer = true;
			}
		}

		if (!m_continueBuffer && !hasCollectStr)
		{
			Logger::log("->1");
			if (index_rule + 1 < m_path.size())
			{
				Logger::log("->2");
				Logger::log("concept: " + LoggerJson::get(m_path[index_rule + 1]->getConceptName()));

				if (!m_path[index_rule + 1]->getConceptPtr()->isTerminalConcept())
				{
					Logger::log("->3");

					if (m_pathStack.has(index_rule + 1))
					{
						if (m_pathStack.get(index_rule + 1) != m_path.slice(index_rule + 1))
						{
							Logger::log("get");
							AJson *currentJson2 = m_mapIndexToJson.get(index_rule + 1);
							m_mapIndexToJson.get(index_rule)->asArray()->push(currentJson2);
							m_mapIndexToJson.remove(index_rule + 1);
						}
					}

					const std::string key = m_path[index_rule + 1]->getConceptName();
					if (!m_mapIndexToJson.has(index_rule + 1))
					{
						Logger::log("create_object");

						m_pathStack.set(index_rule + 1, m_path.slice(index_rule + 1));
						m_mapIndexToJson.set(index_rule + 1, AJson::createObject());
						m_mapIndexToJson.get(index_rule + 1)->asObject()->set(key, AJson::createArray());

						Logger::log("Object created: " + LoggerJson::get(m_mapIndexToJson.get(index_rule + 1)));
					}

					if (!m_buffer.empty())
					{
						if (m_path[index_rule + 1]->isSequence())
						{
							AJson	  *currentJson	 = m_mapIndexToJson.get(index_rule + 1);
							JsonArray *currentObject = currentJson->asObject()->get(m_path[index_rule + 1]->getConceptName())->asArray();
							AJson	  *lastElement	 = currentObject->getLast();
							if (!lastElement || lastElement->getType() != AJson::ARRAY)
								currentObject->push(AJson::createArray());
							currentObject->getLast()->asArray()->push(AJson::createString(m_buffer));
						}
						else
						{
							m_mapIndexToJson.get(index_rule + 1)->asObject()->get(key)->asArray()->push(AJson::createString(m_buffer));
							if (m_path[index_rule]->isSequence() && m_path[index_rule + 1]->isEnd())
							{
								const std::string key2			= m_path[index_rule]->getConceptName();
								JsonArray		 *currentObject = m_mapIndexToJson.get(index_rule)->asObject()->get(key2)->asArray();

								AJson *lastElement = currentObject->getLast();
								if (!lastElement || lastElement->getType() != AJson::ARRAY)
								{
									currentObject->push(AJson::createArray());
									lastElement = currentObject->getLast()->asArray();
								}
								lastElement->asArray()->push(m_mapIndexToJson.get(index_rule + 1));
								m_mapIndexToJson.remove(index_rule + 1);
								m_pathStack.remove(index_rule + 1);
							}
						}
						m_buffer.clear();
					}

					if (m_cidx >= m_pathNexts.size() || (!same_prec(m_path, m_pathNexts[m_cidx], index_rule + 1)))
					{
						Logger::log("Ok56");
						AJson *currentJson2 = m_mapIndexToJson.get(index_rule + 1);
						if (index_rule == 0)
							m_mapIndexToJson.get(index_rule)->asArray()->push(currentJson2);
						else
						{
							m_mapIndexToJson.get(index_rule)->asObject()->get(m_path[index_rule]->getConceptName())->asArray()->push(currentJson2);
						}
						Logger::log("Ok50");
						m_mapIndexToJson.remove(index_rule + 1);
						m_pathStack.remove(index_rule + 1);
					}

					Logger::log("Ok97");


					return;
				}
			}
		}

		if (!m_continueBuffer && index_rule == 0)
		{
			Logger::log("Ok99995");
			*json = AJson::createString(m_buffer);
			return;
		}
		Logger::log("Ok59");

		return;
	}
	else
	{
		Logger::log("Ok6");

		Logger::log("string : " + LoggerJson::get(m_buffer));
		// Si TerminalConcept on doit ajouter des string au create string
		if (m_path[index_rule]->isSequence())
		{
			if (!m_mapIndexToJson.has(index_rule))
				m_mapIndexToJson.set(index_rule, AJson::createArray());
		}
		else
		{
			if (!m_mapIndexToJson.has(index_rule))
				m_mapIndexToJson.set(index_rule, AJson::createArray());
		}

		AJson *currentJson = m_mapIndexToJson.get(index_rule);
		if (currentJson->getType() == AJson::ARRAY)
		{
			currentJson->asArray()->push(ptr);
		}
		if (!same_prec(m_path, m_pathNexts[m_cidx], index_rule - 1))
		{
			*json = currentJson;
			return;
		}
	}
}

void DataCollector::generateJson(const BNFPath *cursor, const std::string &stringToParsed)
{
	// clang-format off

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

	m_continueBuffer = false;

	AJson *arr = AJson::createArray();
	m_mapIndexToJson.set(0, arr);
    m_cidx = 0;
    m_pathNexts = pathStack;
	m_lines = line;

	while (m_cidx < pathStack.size())
	{
        m_path = pathStack[m_cidx];
		Logger::log("=======");
		recurseJson(0, &json, false);
		Logger::log("json : " + LoggerJson::get(json));
		if (!m_continueBuffer)
		{
			m_buffer.clear();
		}
		m_continueBuffer = false;
		if (json)
		{
			m_mapIndexToJson.get(0)->asArray()->push(json);
			json = NULL;
		}
		if (m_mapIndexToJson.has(0))
			Logger::log("arr : " + LoggerJson::get(m_mapIndexToJson.get(0), false));
	}
}

