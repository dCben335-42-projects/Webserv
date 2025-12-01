#include <string>
#include <vector>
#include <sstream>
#include <stdexcept>
#include <iostream>
#include "./SemanticDescriptor.hpp"
#include <algorithm>
#include "../utils/utils.cpp"

using namespace bnf;

static bool is_rule_start(const std::string &line)
{
    size_t i = 0;
    while (i < line.size() && std::isspace(line[i]))
        ++i;

    if (i + 2 <= line.size() && line.compare(i, 2, "|=") == 0)
        return true;

    size_t start = i;
    while (i < line.size() && (std::isalnum(line[i]) || line[i] == '_'))
        ++i;

    if (i == start)
        return false;

    while (i < line.size() && std::isspace(line[i]))
        ++i;

    if (i + 3 <= line.size() && line.compare(i, 3, "::=") == 0)
        return true;

    return false;
}

void SemanticDescriptor::printSemanticDescriptor()
{
    for (std::map<std::string, std::vector<Concept *> *>::iterator it = m_concepts.begin(); it != m_concepts.end(); ++it)
    {
        std::cout << "---------------------" << std::endl;
        const std::string &key = it->first;
        std::vector<Concept *> *conceptPtr = it->second;

        std::cout << "Concept name: " << key << std::endl;
        std::cout << "Number of rules: " << conceptPtr->size() << std::endl;

        if (conceptPtr)
        {
            int index = 0;
            for (std::vector<Concept *>::iterator it = conceptPtr->begin(); it != conceptPtr->end(); ++it, ++index)
            {
                std::cout << "Rule " << index << ":" << std::endl;
                (*it)->printRules();
            }
        }
    }
}

// Fonction utilitaire : découpe le texte en blocs selon is_rule_start
static void splitBlocks(const std::string &text, std::vector<std::string> &blocks)
{
    std::istringstream iss(text);
    std::string line, current_block;

    while (std::getline(iss, line))
    {
        if (is_rule_start(line) && !current_block.empty())
        {
            blocks.push_back(current_block);
            current_block.clear();
        }
        current_block += line + "\n";
    }

    if (!current_block.empty())
        blocks.push_back(current_block);
}

// Fonction utilitaire : trouve la position du séparateur et extrait le nom du concept
static size_t findSeparator(const std::string &block, bool &has_eq)
{
    size_t pos = block.find(EQRULE);
    has_eq = true;

    if (pos == std::string::npos)
    {
        pos = block.find(ORRULE);
        if (pos == std::string::npos)
            throw std::runtime_error("Erreur: '::=' ou '|=' non trouvé");
        has_eq = false;
        pos += ORRULE.length();
    }
    else
    {
        pos += EQRULE.length();
    }

    return pos;
}

// Version compacte de createBnf
void SemanticDescriptor::createBnf(const std::string &m_content)
{
    std::vector<std::string> blocks;
    splitBlocks(m_content, blocks);

    for (size_t i = 0; i < blocks.size(); ++i)
    {
        const std::string &block = blocks[i];
        bool has_eq;
        size_t pos = findSeparator(block, has_eq);

        if (has_eq)
        {
            std::string concept_name = block.substr(0, pos - EQRULE.length());
            parseConceptName(concept_name);
        }

        parseRule(block, pos);
    }
}

void SemanticDescriptor::parseRule(const std::string &m_content, size_t cursor)
{
    std::vector<Concept *> *concepts = m_concepts[m_lastKey];
    Concept *concept = new Concept();
    concepts->push_back(concept);
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

    m_concepts[s] = new std::vector<Concept *>();
    m_lastKey = s;
}

static void printVector(const std::vector<std::string> &vec)
{
    std::cout << "[";
    for (size_t i = 0; i < vec.size(); ++i)
    {
        std::cout << vec[i];
        if (i < vec.size() - 1)
            std::cout << ", ";
    }
    std::cout << "]" << std::endl;
}

typedef std::map<std::string, TreeNodeData> DataMap;

TreeNode *SemanticDescriptor::createTree(TreeNode *node)
{
    if (!node->value.concepts)
        return node;
    std::vector<std::string> items;

    for (std::vector<Concept *>::iterator it = node->value.concepts->begin(); it != node->value.concepts->end(); ++it)
    {
        items = (*it)->getItems();

        for (std::vector<std::string>::iterator itemIt = items.begin(); itemIt != items.end(); ++itemIt)
        {
            m_is_stopped_by_rconcept = false;

            TreeNode* temp_node = new TreeNode(
                getOrInsert(m_data, *itemIt, TreeNodeData(*itemIt, m_concepts[*itemIt]))->second,
                node->path
            );

            if (startsWith(*itemIt, "'"))
            {
                node->children.push_back(temp_node);
                continue;
            }

            if (!m_concepts[*itemIt])
                throw std::runtime_error(ERR_CONCEPT_NOT_FOUND + *itemIt);

            printVector(temp_node->path);

            if (!isInContainer(temp_node->path, *itemIt))
            {
                temp_node->path.push_back(*itemIt);
                node->children.push_back(temp_node);
                createTree(temp_node);

                if (isInMap(m_rconcept_check_result, *itemIt) && !m_is_stopped_by_rconcept)
                {
                    std::cout << "\033[32mRecursive concept validated:\033[0m " << *itemIt << std::endl;
                    m_rconcept_check_result[*itemIt] = true;
                    m_is_stopped_by_rconcept = false;
                }
            }
            else
            {
                // Si on trouve un chemin récursif
                std::cout << "==========================" << std::endl;
                std::cout << "\033[31mRecursive path detected but allowed:\033[0m : " << *itemIt << std::endl;

                // Marquer ce concept comme a verifier
                if (!isInMap(m_rconcept_check_result, *itemIt)) 
                    m_rconcept_check_result[*itemIt] = false;
                m_is_stopped_by_rconcept = true;
            }
        }
    }
    return node;
}

void SemanticDescriptor::checkValidity()
{
    m_is_stopped_by_rconcept = false;

    TreeNode* node = new TreeNode(CURSOR, m_concepts[CURSOR]);
    node->path.push_back(CURSOR);

    createTree(node);

    for (std::map<std::string, bool>::iterator it = m_rconcept_check_result.begin();
     it != m_rconcept_check_result.end();
     ++it)
    {
        std::cout << "clé = " << it->first 
                << ", valeur = " << (it->second ? "true" : "false")
                << std::endl;
        if (!it->second)
            throw std::runtime_error("Error: Recursive path detected: " + it->first);
    }
}

void SemanticDescriptor::printTree(TreeNode* node, int level)
{
    if (!node) return;

    for (int i = 0; i < level; ++i)
        std::cout << "  ";
    std::cout << node->value.value << std::endl;

    for (std::vector<TreeNode*>::iterator it = node->children.begin(); 
         it != node->children.end(); ++it)
    {
        printTree(*it, level + 1);
    }
}
