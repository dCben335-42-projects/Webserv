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

bool DataCollector::samePrecP(const ARulePath &path1, const ARulePath &path2, const size_t n) const
{
	if (path1.size() < n || path2.size() < n)
		return false;
	for (size_t i = 0; i < n; ++i)
	{
		Logger::log(path2[i]);
		if (path1[i] != path2[i])
			return false;
	}
	return true;
}

bool DataCollector::isEnd(size_t idx) const
{
	for (size_t i = idx; i < m_path.size(); ++i)
	{
		if (!m_path[i]->isEnd())
			return false;
	}
	return true;
}

bool DataCollector::isEndP(ARulePath path, size_t idx) const
{
	for (size_t i = idx; i < path.size(); ++i)
		if (!path[i]->isEnd())
			return false;
	return true;
}

int DataCollector::getLastSequence(Vec<ARulePath> pathStack, size_t idx) const
{
	int size = pathStack[idx].size();

	for (int i = size - 1; i >= 0; --i)
	{
		if (pathStack[idx][i]->isSequence())
			return i;
	}
	return -1;
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
	// Logger::log(m_buffer);
}

JsonArray *DataCollector::getObjectArray(const size_t idx, const std::string &key)
{
	return getJson(idx)->asObject()->get(key)->asArray();
}





void DataCollector::recurseJson(size_t idxRule, AJson **json)
{
	AJson			  *ptr		= NULL;
	size_t			   idxRuleC = idxRule + 1;
	const std::string &keyC		= getKey(idxRuleC);
	const std::string &key		= getKey(idxRule);

	if (isLastRule(idxRule))
	{
		appendBuffer();
		if (idxRule == 0)
		{
			*json = AJson::createString(m_buffer);
			m_buffer.clear();
		}
		return;
	}
	recurseJson(idxRuleC, &ptr);

	if (ptr)
	{
		createNewObject(idxRuleC, keyC);
		getObjectArray(idxRuleC, keyC)->push(ptr);
		movJson(getJson(idxRule)->asObject()->get(key), idxRuleC);
		return;
	}

	if (!(idxRuleC < m_path.size() && !isTerminalConcept(idxRuleC)))
	{
		if (idxRule == 0)
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





int DataCollector::getFirstTerminalConcept(ARulePath path) const
{
	for (size_t i = 0; i < path.size(); ++i)
	{
		if (path[i]->getConceptPtr()->isTerminalConcept())
			return i;
	}
	return -1;
}

int DataCollector::getFirstSequence(ARulePath path, size_t max_bottom) const
{
	for (int i = max_bottom; i >= 0; --i)
	{
		if (path[i]->isSequence())
			return i;
	}
	return -1;
}

bool hasWordStartPath(Vec<ARulePath> wordStartPath, ARulePath tempRulePath)
{
	for (size_t k = 0; k < wordStartPath.size(); ++k)
	{
		if (tempRulePath == wordStartPath[k])
			return true;
	}
	return false;
}


bool DataCollector::findCharPos(
	const std::vector<std::string> &lines,
	size_t							globalIndex,
	size_t						   &lineIndex,
	size_t						   &localIndex)
{
	size_t acc = 0;

	for (size_t i = 0; i < lines.size(); ++i)
	{
		size_t len = lines[i].size();

		if (globalIndex < acc + len)
		{
			lineIndex  = i;
			localIndex = globalIndex - acc;
			return true;
		}

		acc += len;
	}

	return false; // out of range
}

Vec<std::string> nlines;
std::string		 str = "";

// clang-format off
void DataCollector::generateJson(const BNFPath *cursor, const std::string &strToParsed, Vec<std::string> lines)
{
	std::string str = "";
	str += strToParsed[0];
	lines.clear();
	size_t l = 0;

	m_pathNexts.push_back(cursor->rulePath);
	while (cursor && cursor->next.size() > 0)
	{
		l++;

		for (size_t i = 0; i < cursor->next.size(); ++i)
		{
			if (!cursor->next[i].isInvalidedPath)
			{
				cursor = &cursor->next[i];
				break;
			}
		}
		m_pathNexts.push_back(cursor->rulePath);
		ARulePath tempRulePath = cursor->rulePath;
		Vec<ARulePath> wordStartPath = cursor->prev->wordStartPath;

		if (hasWordStartPath(wordStartPath, tempRulePath))
		{
			lines.push_back(str); str = "";
		}

		str += strToParsed[cursor->index];
	}
	lines.push_back(str); str = "";

	Logger::log(lines);
	// (void)lines;

	size_t i = 0;

	while (i < m_pathNexts.size())
	{
		size_t line;
		size_t col;

		if (findCharPos(lines, i, line, col))
		{
			size_t size = lines[line].size();
			str.append(lines[line]);
			size_t tempI = i + size;
			if (tempI >= m_pathNexts.size() || getFirstTerminalConcept(m_pathNexts[i]) < 0)
			{
				i = tempI;
				nlines.push_back(str); 
				str = "";
				continue;
			}
			else
			{
				size_t index = getFirstTerminalConcept(m_pathNexts[i]);
				Logger::error(m_pathNexts[tempI]);
		

				if (samePrecP(m_pathNexts[i], m_pathNexts[tempI], index))
				{
					if (index - 1 < 0)
					{
						i = tempI;
						continue;
					}
					
					if (index - 1 >= 0)
					{
						int seqIndex = getFirstSequence(m_pathNexts[i], index - 1);
						if (seqIndex == -1)
						{
							i = tempI;
							continue;
						}
						else
						{
							if (!isEnd(seqIndex + 1))
							{
								i = tempI;
								continue;
							}
						}
					}
				}
				i = tempI;
				nlines.push_back(str); str = "";
			}
		}
	}
	// Logger::log("----------------");
	// Logger::log(nlines);

	AJson *json = NULL;
	AJson *arr = AJson::createArray();
	// BUG pas de tableau creer si repetition sur la <head> line
	m_mapJson.set(0, arr);
    m_cidx = 0;
	m_lines = nlines;

	while (m_cidx < m_pathNexts.size())
	{
		m_collectStrIdx = -1;
        m_path = m_pathNexts[m_cidx];
		recurseJson(0, &json);
		m_buffer.clear();
		if (json)
		{
			m_mapJson.get(0)->asArray()->push(json);
			json = NULL;
		}
		// if (m_mapJson.has(0))
		// 	Logger::log("arr : " + LoggerJson::get(m_mapJson.get(0), false));
	}
	if (m_mapJson.has(0))
		Logger::log("arr : " + LoggerJson::get(m_mapJson.get(0), false));
}

