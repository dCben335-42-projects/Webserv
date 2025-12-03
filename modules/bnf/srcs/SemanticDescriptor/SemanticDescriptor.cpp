#include <string>
#include <vector>
#include <sstream>
#include <stdexcept>
#include <iostream>
#include "./SemanticDescriptor.hpp"
#include <algorithm>

using namespace bnf;

static bool is_rule_start(const std::string &line, size_t *startDef)
{
    size_t i = 0;
    while (i < line.size() && std::isspace(line[i]))
        ++i;
    if (i + 2 <= line.size() && line.compare(i, 2, "|=") == 0)
    {
        *startDef = i + 2;
        return true;
    }
    size_t start = i;
    while (i < line.size() && (std::isalnum(line[i]) || line[i] == '_'))
        ++i;
    if (i == start)
        return false;
    while (i < line.size() && std::isspace(line[i]))
        ++i;
    if (i + 3 <= line.size() && line.compare(i, 3, "::=") == 0)
    {
        *startDef = i + 3;
        return true;
    }
    return false;
}

static void splitBlocks(const std::string &text, std::vector<std::string> &blocks)
{
    std::istringstream iss(text);
    std::string line, current_block;
    size_t startDef = 0, prevStartDef = 0;

    while (std::getline(iss, line))
    {
        bool start = is_rule_start(line, &startDef);

        if (start && !current_block.empty()) {
            if (isOnlyWhitespace(current_block, prevStartDef))
                throw std::runtime_error(ERR_CONCEPT_NOT_DEFINED);
            blocks.push_back(current_block);
            current_block.clear();
            prevStartDef = startDef;
        }

        current_block.append(line);
        current_block.append(1, '\n');
    }

    if (!current_block.empty())
        blocks.push_back(current_block);
}


static size_t findSeparator(const std::string &block, bool &has_eq)
{
    size_t pos;

    if ((pos = block.find(EQRULE)) != std::string::npos) {
        has_eq = true;
        return pos + EQRULE.length();
    }

    if ((pos = block.find(ORRULE)) != std::string::npos) {
        has_eq = false;
        return pos + ORRULE.length();
    }

    throw std::runtime_error(ERR_CONCEPT_NOT_DEFINED);
}
    
SemanticDescriptor::SemanticDescriptor(const std::string& m_content) {
    std::vector<std::string> blocks;
    splitBlocks(m_content, blocks);

    for (size_t i = 0; i < blocks.size(); ++i)
    {
        const std::string &block = blocks[i];
        bool has_eq;
        size_t pos = findSeparator(block, has_eq);
        if (has_eq)
        {
            std::string conceptName = block.substr(0, pos - EQRULE.length());
            if(miniRegex(conceptName, "[a-zA-Z]*"))
                throw std::runtime_error(ERR_INVALID_CONCEPT_NAME); 
            parseConceptName(conceptName);
        }
        parseRule(block, pos);
    }
    this->checkValidity();
}

SemanticDescriptor::~SemanticDescriptor() {}

void SemanticDescriptor::parseRule(const std::string &m_content, size_t cursor)
{
    Concept *concept = new Concept();
    m_concepts[m_lastKey]->push_back(concept);
    std::istringstream iss(m_content.substr(cursor));
    std::string word;
    ARule *arule = NULL;
    while (iss >> word)
    {
        if (ARule::isKeyWord(word, true))
            throw std::runtime_error("Invalid keyword: " + word);
        BnfMap map = ARule::getMap(word);
        arule = map.constructor();
        if (map.hasClosure && !(iss >> word))
            throw std::runtime_error("Unexpected end of rule after " + map.opener);
        while (word != map.closer)
        {
            if (ARule::isKeyWord(word, false))
                throw std::runtime_error("Bad closing keyword: " + word);
            if (word != DIVIDER)
            {
                if(!validate_string_strict(word))
                    throw std::runtime_error(ERR_INVALID_CONCEPT_NAME); 
                arule->addItem(word);
                if (!map.hasClosure)
                    break;
            }
            if (!(iss >> word))
                break;
        }
        concept->setRule(arule);
    }
}

void SemanticDescriptor::parseConceptName(std::string &s)
{
    s.erase(0, s.find_first_not_of(WHITESPACE));
    s.erase(s.find_last_not_of(WHITESPACE) + 1);

    std::istringstream iss(s);
    std::string w;
    if (!(iss >> w) || (iss >> w))
        throw std::runtime_error(ERR_INVALID_CONCEPT_NAME);

    if (m_concepts.find(s) != m_concepts.end())
        throw std::runtime_error(ERR_CONCEPT_ALREADY_DEFINED + s);

    m_concepts[s] = new ConceptList();
    m_lastKey = s;
}

bool SemanticDescriptor::hasRecursiveError(TreeNode *node)
{
    for (ConceptList::iterator it = node->content.concepts->begin();
         it != node->content.concepts->end(); ++it)
    {
        Vec<std::string>::type items = (*it)->getItems();
        bool isLineError = false;

        for (Vec<std::string>::iterator itemIt = items.begin(); itemIt != items.end(); ++itemIt)
        {
            const std::string &item = *itemIt;

            if (startsWith(item, DQSQ))
                continue;

            if (!isInMap(m_concepts, item))
                throw std::runtime_error(ERR_CONCEPT_NOT_FOUND + item);

            TreeNodeData data(item, m_concepts[item]);
            TreeNode* temp_node = new TreeNode(getOrInsert(m_data, item, data)->second, node->path);

            if (!isInContainer(temp_node->path, item))
            {
                temp_node->path.push_back(item);
                node->children.push_back(temp_node);
                if (hasRecursiveError(temp_node))
                    isLineError = true;
            }
            else
            {
                isLineError = true;
                temp_node->path.push_back(item);
            }
        }
        if (!isLineError)
            node->content.isInfinite = false;
    }
    return node->content.isInfinite;
}

void SemanticDescriptor::checkValidity()
{
    TreeNode* node = new TreeNode(CURSOR, m_concepts[CURSOR]);
    if (hasRecursiveError(node))
        throw  std::runtime_error("ERREUR !");
}

Map<ConceptList *>::type SemanticDescriptor::getConcepts() const
{
    return m_concepts;
}

std::ostream& operator<<(std::ostream& os, const SemanticDescriptor& data)
{
    log(data.getConcepts());
    return (os);
}