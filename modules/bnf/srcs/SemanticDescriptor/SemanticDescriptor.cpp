#include <string>
#include <vector>
#include <sstream>
#include <stdexcept>
#include <iostream>
#include "./SemanticDescriptor.hpp"

using namespace bnf;

static bool is_rule_start(const std::string &line)
{
    size_t i = 0;
    while (i < line.size() && std::isspace(line[i]))
        ++i;

    // Cas 1 : vérifie si c'est juste "|="
    if (i + 2 <= line.size() && line.compare(i, 2, "|=") == 0)
        return true;

    // Cas 2 : mot suivi de "::="
    size_t start = i;
    while (i < line.size() && (std::isalnum(line[i]) || line[i] == '_'))
        ++i;

    if (i == start) // pas de mot trouvé
        return false;

    // Sauter les espaces après le mot
    while (i < line.size() && std::isspace(line[i]))
        ++i;

    // Vérifier si on a "::="
    if (i + 3 <= line.size() && line.compare(i, 3, "::=") == 0)
        return true;

    return false;
}



void SemanticDescriptor::printSemanticDescriptor() {
    
    for (std::map<std::string, std::vector<Concept *>*>::iterator it = m_concepts.begin(); it != m_concepts.end(); ++it) {
        std::cout << "---------------------" << std::endl;
        const std::string& key = it->first;
        std::vector<Concept *> *conceptPtr = it->second;

        std::cout << "Concept name: " << key << std::endl;
        std::cout << "Number of rules: " << conceptPtr->size() << std::endl;

        if (conceptPtr)
        {
            int index = 0;
            for (std::vector<Concept*>::iterator it = conceptPtr->begin(); it != conceptPtr->end(); ++it, ++index)
            {
                std::cout << "Rule " << index << ":" << std::endl;
                (*it)->printRules();
            }
        }
    }
}

void SemanticDescriptor::createBnf(const std::string& m_content) {
    std::vector<std::string> lines;
    std::istringstream iss(m_content);
    std::string line;
    std::vector<std::string> blocks;
    std::string current_block;
    while (std::getline(iss, line))
    {
        if (is_rule_start(line))
        {
            if (!current_block.empty())
            {
                blocks.push_back(current_block);
                current_block.clear();
            }
        }
        current_block += line + "\n";
    }

    if (!current_block.empty())
        blocks.push_back(current_block);

    for (size_t i = 0; i < blocks.size(); ++i)
    {
        size_t pos = blocks[i].find(EQRULE);

        if (pos == std::string::npos)
        {
            pos = blocks[i].find(ORRULE);
            if (pos == std::string::npos)
            {
                throw std::runtime_error("Erreur: '::=' non trouvé");
            }
            else
            {
                pos += ORRULE.length();
            }
        }
        else
        {
            std::string a = blocks[i].substr(0, pos);
            parseConceptName(a);
            pos += EQRULE.length();
        }
        parseRule(blocks[i], pos);
    }
}

void SemanticDescriptor::parseRule(const std::string& m_content, size_t cursor) {

    std::string rule;

    m_concepts[m_last_key]->push_back(new Concept());
    
    rule = m_content.substr(cursor);
    std::istringstream iss(rule);
    std::vector<std::string> parts;
    std::string word;

    ARule * arule = NULL; 

    while (iss >> word) {
         if (ARule::isKeyWord(word, true))
            throw std::runtime_error("Error bad closing " + word);
        BnfMap map = ARule::getMap(word);
        arule = map.constructor();
        if (map.hasClosure)
            iss >> word;
        while (word != map.closer)
        {
            if (ARule::isKeyWord(word, false))
                throw std::runtime_error("Error bad closing " + word);
            if (word != DIVIDER)
            {
                arule->addItem(word);
                if (!map.hasClosure)
                    break;
            }
            if (!(iss >> word))
                break;
        }
        (*m_concepts[m_last_key])[(*m_concepts[m_last_key]).size() - 1]->setRule(arule);
    }
}

void SemanticDescriptor::parseConceptName(std::string& m_content) {
    m_content.erase(0, m_content.find_first_not_of(" \t\n\r"));
    m_content.erase(m_content.find_last_not_of(" \t\n\r") + 1);

    std::istringstream iss(m_content);
    std::vector<std::string> parts;
    std::string word;
    while (iss >> word) {
        parts.push_back(word);
    }

    if (parts.size() != 1) {
        throw std::runtime_error("Erreur: clé invalide (espaces internes détectés)");
    }

    m_concepts.insert(std::make_pair(m_content, new std::vector<Concept *>()));
    m_last_key = m_content;
}
