#include <string>
#include <vector>
#include <sstream>
#include <stdexcept>
#include <iostream>
#include "./SemanticDescriptor.hpp"
#include <algorithm>
#include "../utils/utils.hpp"
#include "../utils/debug.cpp"

using namespace bnf;

typedef std::vector<Concept *> ConceptList;
typedef std::map<std::string, TreeNodeData> DataMap;
    
static bool is_rule_start(const std::string &line, size_t *startDef)
{
    size_t i = 0;
    while (i < line.size() && std::isspace(line[i]))
        ++i;
    if (i + 2 <= line.size() && line.compare(i, 2, "|=") == 0)
    {
        std::cout << "1ndex: " <<  i << std::endl;
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
        std::cout << "index: " <<  i << std::endl;
        *startDef = i + 3;
        return true;
    }
    return false;
}

static bool isOnlyWhitespace(const std::string& str, size_t start)
{
    if (start >= str.size())
        return false;
    for (std::string::const_iterator it = str.begin() + start; it != str.end(); ++it) {
        if (!isspace(static_cast<unsigned char>(*it))) {
            return false;  // On a trouvé un caractère qui n'est pas un blanc
        }
    }
    return true;  // Tous les caractères étaient des blancs
}


void SemanticDescriptor::printSemanticDescriptor()
{
    for (std::map<std::string, ConceptList *>::iterator it = m_concepts.begin(); it != m_concepts.end(); ++it)
    {
        std::cout << "---------------------" << std::endl;
        const std::string &key = it->first;
        ConceptList *conceptPtr = it->second;

        std::cout << "Concept name: " << key << std::endl;
        std::cout << "Number of rules: " << conceptPtr->size() << std::endl;

        if (conceptPtr)
        {
            int index = 0;
            for (ConceptList::iterator it = conceptPtr->begin(); it != conceptPtr->end(); ++it, ++index)
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
    size_t startDef = 0;

    while (std::getline(iss, line))
    {
        if (is_rule_start(line, &startDef) && !current_block.empty())
        {
            if (isOnlyWhitespace(current_block, startDef))
            {
                throw std::runtime_error("Benoit a mage ma soupel");
            }
            blocks.push_back(current_block);
            current_block.clear();
            std::cout <<  startDef << std::endl;
        }
        current_block += line + "\n";
        std::cout << current_block << std::endl;
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

bool SemanticDescriptor::createTree(TreeNode *node)
{
    // if (!node->content.concepts || !node->content.concepts->size())
    //     return true;
    Vec<std::string>::type items;
    
    for (ConceptList::iterator it = node->content.concepts->begin(); it != node->content.concepts->end(); ++it)
    {
        items = (*it)->getItems();
        if (!items.size())
            throw std::runtime_error("ERREUR de CON");
        bool isLineError = false;
        for (Vec<std::string>::iterator itemIt = items.begin(); itemIt != items.end(); ++itemIt)
        {
            if (startsWith(*itemIt, DQSQ))
                continue;
            if (!isInMap(m_concepts, *itemIt))
                throw std::runtime_error(ERR_CONCEPT_NOT_FOUND + *itemIt);
            TreeNode* temp_node = new TreeNode(
                getOrInsert(
                    m_data,
                    *itemIt,
                    TreeNodeData(*itemIt, m_concepts[*itemIt])
                )->second,
                node->path
            );

            if (!isInContainer(temp_node->path, *itemIt))
            {
                temp_node->path.push_back(*itemIt);
                node->children.push_back(temp_node);
                if (createTree(temp_node))
                    isLineError = true;
            }
            else {
                isLineError = true;
                temp_node->path.push_back(*itemIt);
            }
        }

        if (!isLineError)
            node->content.isInfinite = false;
    }
    return node->content.isInfinite;
}






















void SemanticDescriptor::checkValidity()
{
    m_is_stopped_by_rconcept = false;

    log("Bonjour\n");
    log(m_concepts);
    
    TreeNode* node = new TreeNode(CURSOR, m_concepts[CURSOR]);
    node->path.push_back(CURSOR);
    printSemanticDescriptor();
    if (createTree(node))
        throw  std::runtime_error("ERREUR !");
}

void SemanticDescriptor::printTree(TreeNode* node, int level)
{
    if (!node) return;

    for (int i = 0; i < level; ++i)
        std::cout << "  ";
    std::cout << node->content.name << std::endl;

    for (std::vector<TreeNode*>::iterator it = node->children.begin(); 
         it != node->children.end(); ++it)
    {
        printTree(*it, level + 1);
    }
}
