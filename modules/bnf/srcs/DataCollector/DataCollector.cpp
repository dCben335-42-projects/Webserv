#include "DataCollector.hpp"

DataCollector::DataCollector()
{
}

DataCollector::~DataCollector()
{
}

bool DataCollector::samePrec(const size_t n) const
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

bool DataCollector::isSequence(const size_t idx) const
{
	return m_path[idx]->isSequence();
}

bool DataCollector::isLastRule(const size_t idx_rule) const
{
	return m_path.size() - 1 == idx_rule;
}

bool DataCollector::isTerminalConcept(const size_t idx_rule) const
{
	return m_path[idx_rule]->getConceptPtr()->isTerminalConcept();
}

AJson *DataCollector::getJson(const size_t idx)
{
	return m_mapJson.get(idx);
}

const std::string &DataCollector::getKey(const size_t idx) const
{
	return m_path[idx]->getConceptName();
}

void DataCollector::delJson(const size_t idx)
{
	m_mapJson.remove(idx);
	m_pathStack.remove(idx);
}

void DataCollector::movJson(AJson *json, const size_t idx)
{
	json->asArray()->push(getJson(idx));
	delJson(idx);
}

void DataCollector::addArrayIfNotExist(JsonArray *currentObject)
{
	AJson *lastElement = currentObject->getLast();
	if (!lastElement || lastElement->getType() != AJson::ARRAY)
		currentObject->push(AJson::createArray());
}

void DataCollector::createNewObject(const size_t idx, const std::string &key)
{
	if (!m_mapJson.has(idx))
	{
		m_pathStack.set(idx, m_path.slice(idx));
		m_mapJson.set(idx, AJson::createObject());
		getJson(idx)->asObject()->set(key, AJson::createArray());
	}
}

bool DataCollector::hasRegisteredPath(const size_t idx) const
{
	return m_pathStack.has(idx) && m_pathStack.get(idx) != m_path.slice(idx);
}

void DataCollector::appendBuffer()
{
	m_cidx += m_lines[0].size();
	m_buffer.append(m_lines[0]);
	m_lines.remove(0);
	Logger::log(m_buffer);
}

JsonArray *DataCollector::getObjectArray(const size_t idx, const std::string &key)
{
	return getJson(idx)->asObject()->get(key)->asArray();
}





void DataCollector::recurseJson(
	size_t	idxRule,
	AJson **json,
	bool	hasCollectStr)
{
	AJson *ptr = NULL;
	if (!hasCollectStr)
	{
		hasCollectStr = isTerminalConcept(idxRule);
		if (hasCollectStr)
			m_collectStrIdx = idxRule;
	}



	// hasCollectStr	= hasCollectStr ? true : isTerminalConcept(idxRule);
	size_t			   idxRuleC = idxRule + 1;
	const std::string &keyC		= getKey(idxRuleC);
	const std::string &key		= getKey(idxRule);

	if (isLastRule(idxRule))
	{
		appendBuffer();

		//	Tous ce qui est sous un hasCollectStr doit sappend ensemble mais on doit differencier 2 repetition dune meme classe dans une sequence
		//	Pour ce faire nous devons utiliser isEnd() sur tout element en dessous de la sequence (sil y en a une) dont lelement constitue le TerminalConcept
		//
		//
		//
		//
		//
		//

		// Si index correspond encore a un index de char et que soit:
		// - on est dans un TerminalConcept, que il n'y a pas de sequence sur le parent et que le parent on est toujours sur le meme noeud parent pour le prochain index

		if (!isEndOfPath() && ((hasCollectStr && !isSequence(idxRule - 1) && samePrec(idxRule)) || samePrec(idxRuleC)))
			m_continueBuffer = true;
		else if (idxRule == 0 || (!isEndOfPath() && !samePrec(idxRule) && !hasCollectStr))
		{
			*json = AJson::createString(m_buffer);
			m_buffer.clear();
		}
		return;
	}
	recurseJson(idxRuleC, &ptr, hasCollectStr);
	Logger::log("----<UP>----");

	if (ptr)
	{
		createNewObject(idxRuleC, keyC);
		getObjectArray(idxRuleC, keyC)->push(ptr);
		movJson(getJson(idxRule)->asObject()->get(key), idxRuleC);
		return;
	}

	if (!m_continueBuffer && hasCollectStr && !isEndOfPath() && samePrec(idxRule))
		m_continueBuffer = true;

	if (!(!m_continueBuffer && !hasCollectStr && idxRuleC < m_path.size() && !isTerminalConcept(idxRuleC)))
	{
		if (!m_continueBuffer && idxRule == 0)
			*json = AJson::createString(m_buffer);
		return;
	}
	if (hasRegisteredPath(idxRuleC))
		movJson(getJson(idxRule), idxRuleC);

	createNewObject(idxRuleC, keyC);

	if (!m_buffer.empty())
	{
		JsonArray *currentArray = getObjectArray(idxRuleC, keyC);
		if (isSequence(idxRuleC))
		{
			addArrayIfNotExist(currentArray);
			currentArray->getLast()->asArray()->create(m_buffer);
		}
		else
		{
			currentArray->create(m_buffer);
			if (isSequence(idxRule) && m_path[idxRuleC]->isEnd())
			{
				currentArray = getObjectArray(idxRule, key);
				addArrayIfNotExist(currentArray);
				movJson(currentArray->getLast(), idxRuleC);
			}
		}
		m_buffer.clear();
	}

	if (isEndOfPath() || !samePrec(idxRuleC))
	{
		AJson *currentJson = getJson(idxRule);
		AJson *target	   = !idxRule ? currentJson : currentJson->asObject()->get(key);
		movJson(target, idxRuleC);
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
                // Logger::log(pathStack[i].back());
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

	// line = 

	Vec<AJson *> elements;
	AJson *json = NULL;

	m_continueBuffer = false;

	AJson *arr = AJson::createArray();
	m_mapJson.set(0, arr);
    m_cidx = 0;
    m_pathNexts = pathStack;
	m_lines = line;

	while (m_cidx < pathStack.size())
	{
		m_collectStrIdx = -1;
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
			m_mapJson.get(0)->asArray()->push(json);
			json = NULL;
		}
		if (m_mapJson.has(0))
			Logger::log("arr : " + LoggerJson::get(m_mapJson.get(0), false));
	}
}

