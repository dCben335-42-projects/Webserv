#include "DataCollector.hpp"

DataCollector::DataCollector()
{
}

DataCollector::~DataCollector()
{
}

/**
 * @brief Checks whether the first `n` elements of the current path and the next path are identical.
 *
 * This function compares the prefix of length `n` between `m_path` and
 * `m_pathNexts[m_cidx]`. If either container has fewer than `n` elements,
 * the function immediately returns false.
 *
 * @param n Number of leading elements to compare.
 * @return true if both paths contain at least `n` elements and all corresponding
 *         elements in the prefix are equal.
 * @return false if sizes are insufficient or any element differs.
 */
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
		{
			// clang-format off
			const Map<size_t, Vec<std::string> > &requiredInJsonMap = m_path[i]->getRequiredInJsonMap();
			// clang-format on
			if (!requiredInJsonMap.empty())
			{
				Logger::debug("Not end : " + getKey(i) + " (idx: " + Logger::get(i) + ") because of requiredInJsonMap: " + Logger::get(requiredInJsonMap));
				return false;
			}
		}
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
	// Logger::log("appendBuffer : " + m_lines[0].str + " (required: " + Logger::get(m_lines[0].is_required) + ")");
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
	// Logger::log(m_path[idxRule]);
	// Logger::log(m_pathNexts[m_cidx][idxRule]);
	// Pas sure pour isSequence, isEnd plus approprie

	// bool isSeq = isSequence(idxRule);
	// bool end   = isEnd(idxRule + 1);
	// if (idxRule >= m_pathNexts.size())
	// 	return true;
	// const bool isSeq = isSequence(idxRule);

	// if (isSeq)
	// {
	// 	Logger::info("isSequence : " + getKey(idxRule) + " (idx: " + Logger::get(idxRule) + ")");

	// 	const std::string &cptr		= m_path[idxRule + 1]->getConceptName();
	// 	bool			   isAllENd = true;
	// 	size_t			   i		= idxRule + 1;
	// 	while (i < m_mapJson.size() && i < m_pathNexts[m_cidx].size() && m_pathNexts[m_cidx][i]->getConceptName() == cptr)
	// 	{
	// 		if (!isEnd(i))
	// 		{
	// 			Logger::debug("Not end : " + getKey(i) + " (idx: " + Logger::get(i) + ")");
	// 			isAllENd = false;
	// 			break;
	// 		}
	// 		++i;
	// 	}
	// 	if (isAllENd)
	// 	{
	// 		Logger::debug("isLastWritable (sequence) : " + getKey(idxRule) + " (idx: " + Logger::get(idxRule) + ")");
	// 		return true;
	// 	}
	// }
	// if (!samePrec(idxRule + 1))
	// {
	// 	Logger::debug("Not samePrec for idxRule: " + Logger::get(idxRule + 1));
	// 	return isEnd(idxRule + 1);
	// }
	return samePrec(idxRule + 1) && isEnd(idxRule + 1);
}





void DataCollector::recurseJson(size_t idxRule)
{
	size_t			   idxRuleC = idxRule + 1;
	const std::string &keyC		= getKey(idxRuleC);
	const std::string &key		= getKey(idxRule);
	if (key == "user_name")
		Logger::debug("recurseJson : [idx: " + Logger::get(idxRule) + "]: " + key);
	// Logger::log("recurseJson : [idx: " + Logger::get(idxRule) + "]: " + key);

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
	// if ((int) idxRule > m_requiredIdx)
	// 	return;

	createNewObject(idxRule, key);

	if (!m_buffer.empty())
	{
		getJson(idxRule)->asObject()->get(key)->asArray()->create(m_buffer);
		m_buffer.clear();
	}
	// (void) m_write;

	// if (isLastWritable(idxRule))
	// {
	// 	Logger::log("Writable : " + key + " (idx: " + Logger::get(idxRule) + ")");
	// 	// if (idxRule == 0 && m_write)
	// 	// 	movJson(getJson(0)->asArray(), 1);
	// 	Logger::log("check if can move json from idx " + Logger::get(idxRule - 1) + " to idx " + Logger::get(idxRule));
	// 	if (m_mapJson.has(idxRule - 1) && m_mapJson.has(idxRule))
	// 	{
	// 		Logger::log("last writable : move json from idx " + Logger::get(idxRule - 1) + " to idx " + Logger::get(idxRule));
	// 		movJson(getJson(idxRule - 1)->asObject()->get(getKey(idxRule - 1)), idxRule);
	// 		idxRule -= 1;

	// 		Logger::log("Is idxRule: " + Logger::get(idxRule) + ", key: " + key + " writable ? ");
	// 		if (isLastWritable(idxRule))
	// 		{
	// 			Logger::log("TEST : idxRule: " + Logger::get(idxRule) + " is writable");
	// 		}
	// 	}
	// }

	(void) m_write;





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
	if (!m_bindTerminalConcept)
		return -1;
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

bool hasWordStartPath(Vec<ARulePath> &wordStartPath, const ARulePath &tempRulePath)
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
	if (path[path.size() - 1]->getRequiredInJsonMap().size() > 0)
	{
		// interdire les { 0>'word' } [ 0>'word' ] ( 0>'word' ) Selection interdite sur mot literal dans ces cas
		return (FirstRequired) {(int) path.size() - 1, path[path.size() - 1]->getRequiredInJsonMap().begin()->first};
	}
	return (FirstRequired) {-1, 0};
}

bool DataCollector::shouldContinueConcat(size_t i, size_t tempI, int index) const
{
    if (!samePrecP(m_pathNexts[i], m_pathNexts[tempI], index))
        return false;

    if (index <= 0)
        return true;

    int seqIndex = getFirstSequence(m_pathNexts[i], index - 1);
    if (seqIndex == -1)
        return true;

    return !isEndP(m_pathNexts[i], seqIndex + 1);
}

// void DataCollector::clearPtr(size_t tempI, size_t index, size_t i, bool isStart)
// {
// 	if (!isStart)
// 		m_indexSaved = index;
// 	if (tempI < m_pathNexts.size() && (!samePrecP(m_pathNexts[i], m_pathNexts[tempI], index) || (m_pathNexts[i][index -1]->isSequence() && !isEndP(m_pathNexts[i], index))))
// 	{
// 		if (m_concatPtr.has(index))
// 			m_concatPtr.get(index).clear();
// 		if (isStart)
// 			m_indexSaved = -1;
// 	}
// }

bool DataCollector::shouldClearConcat(size_t tempI, int index, size_t i) const
{
	// Logger::log("shouldClearConcat : tempI: " + Logger::get(tempI) + ", index: " + Logger::get(index) + ", i: " + Logger::get(i));
	if (index < 0)
		return false;
	// Logger::log("shouldClearConcat : tempI: " + Logger::get(tempI) + ", index: " + Logger::get(index) + ", i: " + Logger::get(i));

	if (tempI >= m_pathNexts.size())
		return true;

	// Logger::log("shouldClearConcat : tempI: " + Logger::get(tempI) + ", index: " + Logger::get(index) + ", i: " + Logger::get(i));


	if (!samePrecP(m_pathNexts[i], m_pathNexts[tempI], index))
		return true;

	// Logger::log("shouldClearConcat : tempI: " + Logger::get(tempI) + ", index: " + Logger::get(index) + ", i: " + Logger::get(i));

	
	
	if (index > 0 &&
		m_pathNexts[i][index - 1]->isSequence() &&
		isEndP(m_pathNexts[i], index))
		return true;
		
	int seq = getFirstSequence(m_pathNexts[i], index);
	bool to = seq < 0 ? false : isEndP(m_pathNexts[i], seq + 1);

	return to;
	// Logger::log("shouldClearConcat : tempI: " + Logger::get(tempI) + ", index: " + Logger::get(index) + ", i: " + Logger::get(i));

	return false;
}

// void DataCollector::clearConcat(int index)
// {
//     if (index >= 0 && m_concatPtr.has(index))
//         m_concatPtr.get(index).clear();
// }

void DataCollector::clearConcat(int index)
{
    if (index < 0)
        return;

    std::map<size_t, Map<size_t, size_t> >::iterator it = m_concatPtr.lower_bound(index);

    while (it != m_concatPtr.end())
    {
		Logger::log("clearConcat : index: " + Logger::get(index) + ", it->first: " + Logger::get(it->first));
        std::map<size_t, Map<size_t, size_t> >::iterator toErase = it++;
        m_concatPtr.erase(toErase);
    }
}



Vec<StringNode> DataCollector::stringOrganiser(const std::vector<std::string> &lines)
{
	size_t i = 0;
	Vec<StringNode> nlines;
	m_indexSaved = -1;
	std::string str = "";
	int index = -1;

	while (i < m_pathNexts.size())
	{
		size_t line;
		size_t col;
		size_t tempI;

		if (!findCharPos(lines, i, line, col))
			break;

		size_t size = lines[line].size();
		str.append(lines[line]);
		tempI = i + size;
		Logger::log("str : " + Logger::get(str));
		Logger::log(m_concatPtr);

		// if (tempI >= m_pathNexts.size() && )
		// 	return true;
		if(shouldClearConcat(tempI, index, i))
			clearConcat(index);

		index = getFirstTerminalConcept(m_pathNexts[i]);
		Logger::log("index : " + Logger::get(index));
		FirstRequired required = getFirstRequiredInJsonMap(m_pathNexts[i]);

		Logger::log("ops001");

		if (tempI >= m_pathNexts.size() || (index & required.index) < 0)
		{
			Logger::log("ops00");
			i = tempI;
			nlines.push(StringNode(str, required.index));
			index = required.index;
			str.clear();
			continue;
		}
		else
		{
			Logger::log("ops002");
			if (required.index >= 0 && (required.index < index || index == -1))
			{
				Logger::log("ops0025");

				// Logger::log("ops2");
				index = required.index;
				// Logger::log("index : " + Logger::get(index));
				// Logger::log(m_pathNexts[i]);
				// Logger::log(!isEndP(m_pathNexts[i], index + 1));
				// Logger::log(!m_pathNexts[i][index - 1]->isSequence());
				// // Si on est pas a la fin mais que le prochain element a pour parent le meme que l'actuel
				// // Ne prend pas en compte les optional
				// int seq = getFirstSequence(m_pathNexts[i], index);
				// bool to = seq < 0 ? true : !m_pathNexts[i][seq + 1]->isEnd();

				int seq = getFirstSequence(m_pathNexts[i], index);
				bool to = seq < 0 ? true : !isEndP(m_pathNexts[i], seq + 1);
				Logger::error("samePrecP : " + Logger::get(samePrecP(m_pathNexts[i], m_pathNexts[tempI], index + 1)));


				if (!isEndP(m_pathNexts[i], index + 1) || (samePrecP(m_pathNexts[i], m_pathNexts[tempI], index + 1) && to))
				{
					Logger::log("ops3");

					i = tempI;
					continue;
				}
			}
			else if (shouldContinueConcat(i, tempI,index))
			{
				Logger::log("ops");
				i = tempI;
				continue;
			}
			
			if (required.index >= 0)
			{
				Logger::log("ops8");

				if (!m_concatPtr.has(index))
					m_concatPtr.set(index, Map<size_t, size_t>());

				if (!m_concatPtr.get(index).has(required.channel))
				{
					Logger::log("ops9");

					nlines.push(StringNode(str, required.index)); 
					str.clear();
					m_concatPtr.get(index).set(required.channel, nlines.size() - 1);
				}
				else
				{
					Logger::log("ops10");

					size_t idx = m_concatPtr.get(index).get(required.channel);
					StringNode &ptr = nlines[idx];
					ptr.str += str;
					Logger::log("ptr : " + Logger::get(ptr));
					str.clear();
				}
			}
			else
			{
				nlines.push(StringNode(str, required.index)); 
				str.clear();
			}

		}
		if(shouldClearConcat(tempI, index, i))
			clearConcat(index);
		i = tempI;
	}
	return nlines;
}

// clang-format off
void DataCollector::generateJson(const BNFPath *cursor, const std::string &strToParsed)
{
	m_bindTerminalConcept = false;

	std::string str(1, strToParsed[0]);
	Vec<std::string> lines;

	m_pathNexts.push_back(cursor->rulePath);
	while (cursor && cursor->next.size() > 0)
	{
		for (size_t i = 0; i < cursor->next.size(); ++i)
		{
			if (!cursor->next[i].isInvalidedPath)
			{
				cursor = &cursor->next[i];
				break;
			}
		}
		m_pathNexts.push_back(cursor->rulePath);
		if (hasWordStartPath(cursor->prev->wordStartPath, cursor->rulePath))
		{
			lines.push_back(str); 
			str = "";
		}
		str += strToParsed[cursor->index];
	}
	lines.push_back(str);

	Vec<StringNode> nlines = stringOrganiser(lines);
	Logger::log("nlines : " + Logger::get(nlines));

	Logger::log(m_concatPtr);

	(void) m_collectStrIdx;;
}



std::ostream &operator<<(std::ostream &os, const StringNode &node)
{
	os << "StringNode(str: " << node.str << ", is_required: " << node.is_required << ")";
	return os;
}