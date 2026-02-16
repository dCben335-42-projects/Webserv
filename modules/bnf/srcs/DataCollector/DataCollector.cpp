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
	// Logger::log("movJson : [idx: " + Logger::get(idx) + "]: " + LoggerJson::get(getJson(idx), false) + " to " + LoggerJson::get(json, false));
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
		// Logger::log("createNewObject : [idx: " + Logger::get(idx) + "]: " + key);
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
	Logger::log("appendBuffer : " + m_lines[0].str + " (required: " + Logger::get(m_lines[0].is_required) + ")");
	m_cidx += m_lines[0].str.size();
	m_requiredIdx = m_lines[0].is_required;
	if (m_requiredIdx >= 0)
		m_buffer.append(m_lines[0].str);
	m_lines.remove(0);
	// Logger::log(m_buffer);
}

JsonArray *DataCollector::getObjectArray(const size_t idx, const std::string &key)
{
	return getJson(idx)->asObject()->get(key)->asArray();
}

bool DataCollector::isLastWritable(size_t idxRule) const
{
	// const ARule *path = m_path[idxRule]->getNextRule();
	// Regarder le chemin de la string suivante et verifier quelle n'a pas de lien avec le chemin actuel
	// Peut casser

	// Logger::log(m_path[idxRule + 1]);
	// Logger::log(m_pathNexts[m_cidx][idxRule + 1]);



	if (!samePrec(idxRule + 1))
	{
		Logger::log("isLastWritable : [idx: " + Logger::get(idxRule) + "]: true");
		return true;
	}
	Logger::log("isLastWritable : [idx: " + Logger::get(idxRule) + "]: false");
	return false;
}


void DataCollector::recurseJson(size_t idxRule)
{
	size_t			   idxRuleC = idxRule + 1;
	const std::string &keyC		= getKey(idxRuleC);
	const std::string &key		= getKey(idxRule);

	if (isLastRule(idxRule))
	{
		appendBuffer();
		if (idxRule == 0 && !m_buffer.empty())
		{
			m_mapJson.get(0)->asArray()->create(m_buffer);
			m_buffer.clear();
		}
		return;
	}
	recurseJson(idxRuleC);
	if ((int) idxRule > m_requiredIdx)
		return;

	createNewObject(idxRule, key);
	Logger::log("createNewObject : [idx: " + Logger::get(idxRule) + "]: " + key);

	if (!m_buffer.empty())
	{
		getJson(idxRule)->asObject()->get(key)->asArray()->create(m_buffer);
		m_buffer.clear();
	}

	// Logger::log(m_path[idxRule]);

	if (isLastWritable(idxRule))
	{
		Logger::log("jdjwd : " + key);
		if (m_mapJson.has(idxRule - 1) && m_mapJson.has(idxRule))
		{
			Logger::log("ok");
			Logger::log(getJson(idxRule - 1));
			Logger::log(getJson(idxRule));

			// AJson *currentJson = getJson(idxRule);
			movJson(getJson(idxRule - 1)->asObject()->get(getKey(idxRule - 1)), idxRule);
		}
	}


	// Logger::log("ok");





	(void) keyC;
	(void) key;
	(void) idxRuleC;





	// if (ptr)
	// {
	// 	Logger::log("ok1");
	// 	createNewObject(idxRuleC, keyC);
	// 	getObjectArray(idxRuleC, keyC)->push(ptr);
	// 	movJson(getJson(idxRule)->asObject()->get(key), idxRuleC);
	// 	return;
	// }

	// if (!(idxRuleC < m_path.size() && !isTerminalConcept(idxRuleC)))
	// {
	// 	Logger::log("ok2");

	// 	if (idxRule == 0)
	// 		*json = AJson::createString(m_buffer);
	// 	return;
	// }
	// if (hasRegisteredPath(idxRuleC))
	// {
	// 	Logger::log("ok3");
	// 	movJson(getJson(idxRule), idxRuleC);
	// }

	// createNewObject(idxRuleC, keyC);

	// // Logger::log(m_path[idxRule]->getConceptName() + " : " + Logger::get(idxRuleC) + " : " + Logger::get(m_pathStack.get(idxRuleC).size()));


	// if (!m_buffer.empty())
	// {
	// 	Logger::log("ok4");
	// 	JsonArray *currentArray = getObjectArray(idxRuleC, keyC);
	// 	if (isSequence(idxRuleC))
	// 	{
	// 		addArrayIfNotExist(currentArray);
	// 		currentArray->getLast()->asArray()->create(m_buffer);
	// 	}
	// 	else
	// 	{
	// 		currentArray->create(m_buffer);
	// 		if (isSequence(idxRule) && m_path[idxRuleC]->isEnd())
	// 		{
	// 			currentArray = getObjectArray(idxRule, key);
	// 			addArrayIfNotExist(currentArray);
	// 			movJson(currentArray->getLast(), idxRuleC);
	// 		}
	// 	}
	// 	m_buffer.clear();
	// }

	// if (isEndOfPath() || !samePrec(idxRuleC))
	// {
	// 	Logger::log("ok5");
	// 	AJson *currentJson = getJson(idxRule);
	// 	AJson *target	   = !idxRule ? currentJson : currentJson->asObject()->get(key);
	// 	movJson(target, idxRuleC);
	// }
	// Logger::log("ok6");
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

// clang-format off
FirstRequired DataCollector::getFirstRequiredInJsonMap(ARulePath path) const
{
	for (size_t i = 0; i < path.size() - 1; ++i)
	{
		if (path[i]->getRequiredInJsonMap().size() > 0)
		{
			if (i + 1 < path.size())
			{
				const Map<size_t, Vec<std::string> > &map = path[i]->getRequiredInJsonMap();
				for (Map<size_t, Vec<std::string> >::const_iterator it = map.begin(); it != map.end(); ++it)
				{
					if (it->second.contains(path[i + 1]->getConceptName()))
					{
						return (FirstRequired) {i, it->first};
					}
				}
			}
		}
	}
	return (FirstRequired) {-1, 0};
}


// Vec<std::string> nlines;




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
	lines.push_back(str);
	str = "";

	// Logger::log(lines);

	size_t i = 0;

	Map<size_t, Map<size_t, size_t> > concatPtr;
	Vec<StringNode> nlines;


	Logger::log(lines);
	while (i < m_pathNexts.size())
	{
		size_t line;
		size_t col;

		if (findCharPos(lines, i, line, col))
		{
			size_t size = lines[line].size();
			str.append(lines[line]);
			size_t tempI = i + size;
			if (tempI >= m_pathNexts.size() || (getFirstTerminalConcept(m_pathNexts[i]) < 0 && getFirstRequiredInJsonMap(m_pathNexts[i]).index < 0))
			{
				i = tempI;
				nlines.push(StringNode(str, -1));
				str.clear();
				continue;
			}
			else
			{
				int index = getFirstTerminalConcept(m_pathNexts[i]);
				FirstRequired required = getFirstRequiredInJsonMap(m_pathNexts[i]);

				if (required.index >= 0 && (required.index < index || index == -1))
				{
					index = required.index;

					if (!isEndP(m_pathNexts[i], index + 1) || samePrecP(m_pathNexts[i], m_pathNexts[tempI], index + 1))
					{	
						i = tempI;
						continue;
					}
				}
				else
				{
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
								if (!isEndP(m_pathNexts[i], seqIndex + 1))
								{
									i = tempI;
									continue;
								}
							}
						}
					}
				}
				
				
				if (required.index >= 0)
				{
					if (!concatPtr.has(index))
						concatPtr.set(index, Map<size_t, size_t>());

					if (!concatPtr.get(index).has(required.channel))
					{
						nlines.push(StringNode( str, required.index)); 
						str.clear();
						concatPtr.get(index).set(required.channel, nlines.size() - 1);
					}
					else
					{
						size_t idx = concatPtr.get(index).get(required.channel);
						StringNode &ptr = nlines[idx];
						ptr.str += str;
						str.clear();
					}
				}
				else
				{
					nlines.push(StringNode(str, required.index)); 
					str.clear();
				}
				if (isEndP(m_pathNexts[i], index) && !samePrecP(m_pathNexts[i], m_pathNexts[tempI], index))
				{
					if (concatPtr.has(index))
						concatPtr.get(index).clear();
				}
				i = tempI;
			}
		}
	}
	Logger::log("----------------------");

	for (size_t i = 0; i < nlines.size(); ++i)
	{
		if (nlines[i].is_required < 0)
			Logger::info("nlines : " + nlines[i].str + "\n(required: " + Logger::get(nlines[i].is_required) + ")");
		else
			Logger::warn("nlines : " + nlines[i].str + "\n(required: " + Logger::get(nlines[i].is_required) + ")");
	}















	// Logger::log("nlines : " + Logger::get(nlines));
	// Logger::log("-------------------");
	(void) m_collectStrIdx;;

	// AJson *json = NULL;
	AJson *arr = AJson::createArray();
	// BUG pas de tableau creer si repetition sur la <head> line
	m_mapJson.set(0, arr);
    m_cidx = 0;
	m_lines = nlines;

	while (m_cidx < m_pathNexts.size())
	{
		m_collectStrIdx = -1;
        m_path = m_pathNexts[m_cidx];
		recurseJson(0);
		Logger::log("json : " + LoggerJson::get(m_mapJson, false));
		Logger::log("-------------------");
	}
	// if (m_mapJson.has(0))
	// 	Logger::log("arr : " + LoggerJson::get(m_mapJson.get(0), false));
}

