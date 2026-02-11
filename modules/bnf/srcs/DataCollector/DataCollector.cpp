#include "DataCollector.hpp"

DataCollector::DataCollector()
{
}

DataCollector::~DataCollector()
{
}

bool DataCollector::same_prec(const size_t n) const
{
	if (m_path.size() < n || m_pathNexts[m_cidx].size() < n)
		return false;

	for (size_t i = 0; i < n; ++i)
		if (m_path[i] != m_pathNexts[m_cidx][i])
			return false;
	return true;
}


bool DataCollector::isEndOfPath() const
{
	return m_cidx >= m_pathNexts.size();
}

bool DataCollector::hasParentSequence(size_t idx) const
{
	return m_path[idx - 1]->isSequence();
}

bool DataCollector::isLastRule(size_t idx_rule) const
{
	return m_path.size() - 1 == idx_rule;
}

bool DataCollector::isTerminalConcept(size_t idx_rule) const
{
	return m_path[idx_rule]->getConceptPtr()->isTerminalConcept();
}

void DataCollector::recurseJson(
	size_t	idx_rule,
	AJson **json,
	bool	hasCollectStr)
{
	AJson *ptr		  = NULL;
	hasCollectStr	  = hasCollectStr ? true : isTerminalConcept(idx_rule);
	size_t idx_rule_c = idx_rule + 1;

	if (isLastRule(idx_rule))
	{
		m_cidx += m_lines[0].size();
		m_buffer.append(m_lines[0]);
		m_lines.remove(0);
		Logger::log(m_buffer);

		if (!isEndOfPath() && ((hasCollectStr && !hasParentSequence(idx_rule) && same_prec(idx_rule)) || same_prec(idx_rule_c)))
			m_continueBuffer = true;
		else if (idx_rule == 0 || (!isEndOfPath() && !same_prec(idx_rule) && !hasCollectStr))
		{
			*json = AJson::createString(m_buffer);
			m_buffer.clear();
		}
		return;
	}
	recurseJson(idx_rule_c, &ptr, hasCollectStr);
	Logger::log("----<UP>----");

	if (!ptr)
	{
		if (hasCollectStr && !m_continueBuffer)
		{
			if (!isEndOfPath() && same_prec(idx_rule))
			{
				m_continueBuffer = true;
			}
		}

		if (!m_continueBuffer && !hasCollectStr)
		{
			Logger::log("->1");
			if (idx_rule_c < m_path.size())
			{
				Logger::log("->2");
				Logger::log("concept: " + LoggerJson::get(m_path[idx_rule_c]->getConceptName()));

				if (!isTerminalConcept(idx_rule_c))
				{
					Logger::log("->3");

					if (m_pathStack.has(idx_rule_c))
					{
						if (m_pathStack.get(idx_rule_c) != m_path.slice(idx_rule_c))
						{
							AJson *currentJson2 = m_mapIndexToJson.get(idx_rule_c);
							m_mapIndexToJson.get(idx_rule)->asArray()->push(currentJson2);
							m_mapIndexToJson.remove(idx_rule_c);
						}
					}

					const std::string key = m_path[idx_rule_c]->getConceptName();
					if (!m_mapIndexToJson.has(idx_rule_c))
					{
						m_pathStack.set(idx_rule_c, m_path.slice(idx_rule_c));
						m_mapIndexToJson.set(idx_rule_c, AJson::createObject());
						m_mapIndexToJson.get(idx_rule_c)->asObject()->set(key, AJson::createArray());
					}

					if (!m_buffer.empty())
					{
						if (m_path[idx_rule_c]->isSequence())
						{
							AJson	  *currentJson	 = m_mapIndexToJson.get(idx_rule_c);
							JsonArray *currentObject = currentJson->asObject()->get(m_path[idx_rule_c]->getConceptName())->asArray();
							AJson	  *lastElement	 = currentObject->getLast();
							if (!lastElement || lastElement->getType() != AJson::ARRAY)
								currentObject->push(AJson::createArray());
							currentObject->getLast()->asArray()->push(AJson::createString(m_buffer));
						}
						else
						{
							m_mapIndexToJson.get(idx_rule_c)->asObject()->get(key)->asArray()->push(AJson::createString(m_buffer));
							if (m_path[idx_rule]->isSequence() && m_path[idx_rule_c]->isEnd())
							{
								const std::string key2			= m_path[idx_rule]->getConceptName();
								JsonArray		 *currentObject = m_mapIndexToJson.get(idx_rule)->asObject()->get(key2)->asArray();

								AJson *lastElement = currentObject->getLast();
								if (!lastElement || lastElement->getType() != AJson::ARRAY)
								{
									currentObject->push(AJson::createArray());
									lastElement = currentObject->getLast()->asArray();
								}
								lastElement->asArray()->push(m_mapIndexToJson.get(idx_rule_c));
								m_mapIndexToJson.remove(idx_rule_c);
								m_pathStack.remove(idx_rule_c);
							}
						}
						m_buffer.clear();
					}

					if (isEndOfPath() || (!same_prec(idx_rule_c)))
					{
						AJson *currentJson2 = m_mapIndexToJson.get(idx_rule_c);
						if (idx_rule == 0)
							m_mapIndexToJson.get(idx_rule)->asArray()->push(currentJson2);
						else
							m_mapIndexToJson.get(idx_rule)->asObject()->get(m_path[idx_rule]->getConceptName())->asArray()->push(currentJson2);
						m_mapIndexToJson.remove(idx_rule_c);
						m_pathStack.remove(idx_rule_c);
					}
					return;
				}
			}
		}

		if (!m_continueBuffer && idx_rule == 0)
			*json = AJson::createString(m_buffer);
		return;
	}
	else
	{
		// Si TerminalConcept on doit ajouter des string au create string
		if (m_path[idx_rule]->isSequence())
		{
			if (!m_mapIndexToJson.has(idx_rule))
				m_mapIndexToJson.set(idx_rule, AJson::createArray());
		}
		else
		{
			if (!m_mapIndexToJson.has(idx_rule))
				m_mapIndexToJson.set(idx_rule, AJson::createArray());
		}

		AJson *currentJson = m_mapIndexToJson.get(idx_rule);
		if (currentJson->getType() == AJson::ARRAY)
			currentJson->asArray()->push(ptr);
		if (!same_prec(idx_rule - 1))
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

