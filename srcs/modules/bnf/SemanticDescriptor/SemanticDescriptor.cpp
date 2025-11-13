#include <string>
#include <vector>
#include <sstream>
#include <stdexcept>
#include <iostream>
#include "SemanticDescriptor.hpp"
#include "ARule.hpp"
#include "Symbol.hpp"
#include "bnf_defs.hpp"
#include "Group.hpp"
#include "Sequence.hpp"
#include "Option.hpp"

using bnf::EQRULE;
using bnf::LBRACE;
using bnf::RBRACE;
using bnf::LBRACKET;
using bnf::RBRACKET;
using bnf::DIVIDER;
using bnf::LHOOK;
using bnf::RHOOK;
using bnf::ORRULE;

// static bool is_rule_start(const std::string &line)
// {
//     // Ignore les espaces au début
//     size_t i = 0;
//     while (i < line.size() && std::isspace(line[i]))
//         ++i;

//     // Il faut un mot
//     size_t start = i;
//     while (i < line.size() && (std::isalnum(line[i]) || line[i] == '_'))
//         ++i;

//     if (i == start) // pas de mot trouvé
//         return false;

//     // Sauter les espaces après le mot
//     while (i < line.size() && std::isspace(line[i]))
//         ++i;

//     // Vérifier si on a ::= ou |=
//     if (i + 2 <= line.size() && line.compare(i, 3, "::=") == 0)
//         return true;
//     if (i + 2 <= line.size() && line.compare(i, 2, "|=") == 0)
//         return true;

//     return false;
// }


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



// void SemanticDescriptor::printSemanticDescriptor() {
//     for (std::map<std::string, Concept*>::iterator it = m_concepts.begin(); it != m_concepts.end(); ++it) {
//         const std::string& key = it->first;
//         Concept* conceptPtr = it->second;

//         std::cout << key << " -> " << std::endl;

//         if (conceptPtr)
//             conceptPtr->printRules();  // appelle la fonction de Concept
//     }
// }

void SemanticDescriptor::createBnf(const std::string& m_content) {
    std::vector<std::string> lines;
    std::istringstream iss(m_content);
    std::string line;

    // while (std::getline(iss, line)) { // getline découpe sur '\n' par défaut
    //     lines.push_back(line);
    //     std::cout << line << std::endl;
    // }

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


    size_t concept_count = 0;
    for (size_t i = 0; i < 3; ++i)
    {
        size_t pos = m_content.find(EQRULE);
        if (pos == std::string::npos)
        {
            pos = m_content.find(ORRULE);
            if (pos == std::string::npos)
            {
                throw std::runtime_error("Erreur: '::=' non trouvé");
            }
            else
            {
                pos += ORRULE.length();
                concept_count++;
            }
        }
        else
        {
            std::string a = m_content.substr(0, pos);
            parseConceptName(a);
            concept_count = 0;
        }

        parseRule(blocks[i], pos, concept_count);
    }


    // size_t pos = parseConceptName(blocks[0]);
    // parseRule(blocks[0], pos);



}


// void func(std::string word)
// {
//     if (word == LBRACE)
//     {
//         is_symbol = false;
//         arule = new Group();
//     }
// }

// struct BnfMap 
// {
//     std::string closer;
//     std::string opener;
//     ARule       *rule;
//     ERuleType   ruleType;
// }



// static const BnfMap[] = {
    //     {
        //         .closer = "(",
        //         .opner = ")",
        //         .ruleType = GROUP,
        //         .rule = &Group,
        //     }
        // } 
        
        


size_t find_first_of_any(const std::string& s, const std::string* patterns, size_t count)
{
    size_t pos = std::string::npos;
    for (size_t i = 0; i < count; ++i)
    {
        size_t p = s.find(patterns[i]);
        if (p != std::string::npos && (pos == std::string::npos || p < pos))
            pos = p;
    }
    return pos;
}

size_t SemanticDescriptor::parseRule(const std::string& m_content, size_t cursor, size_t concept_count) {

    std::string rule;

    // size_t pos = m_content.find('\n');

    // const std::string separators[] = { "\n\n", "::=", "|=", "\n" };
    // const size_t sep_count = sizeof(separators) / sizeof(separators[0]);

    // size_t pos = find_first_of_any(m_content, separators, sep_count);
    // if (pos == std::string::npos)
    // else
    //     rule = m_content.substr(cursor, pos);
    
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
        // m_concepts[m_last_key][concept_count]->setRule(arule);
        (*m_concepts[m_last_key])[concept_count]->setRule(arule);

      
        // if (word == LBRACE)
        // {
        //     is_symbol = false;
        //     arule = new Group();
        // }
        // else if (word == LBRACKET)
        // {
        //     is_symbol = false;
        //     arule = new Sequence();
        // }
        // else if (word == LHOOK)
        // {
        //     is_symbol = false;
        //     arule = new Option();
        // }
        // if (word == val.closer)
        // {
        //     m_concepts[m_last_key]->setRule(arule);
        //     arule = NULL;
        // }
        // else if (word == DIVIDER)
        // {
        //     continue;
        // }
        // else if (val.closer == NULL)
        // {
        //     m_concepts[m_last_key]->setRule(new Symbol(word));
        // }
        // else
        //     arule->addItem(word);
    }
    


    return 5;
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

    std::cout << m_content << std::endl;

    m_concepts.insert(std::make_pair(m_content, new std::vector<Concept *>()));
    m_concepts[m_last_key]->push_back(new Concept());
    m_last_key = m_content;
}
