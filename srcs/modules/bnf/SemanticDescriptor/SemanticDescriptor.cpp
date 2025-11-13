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

void SemanticDescriptor::printSemanticDescriptor() {
    for (std::map<std::string, Concept*>::iterator it = m_concepts.begin(); it != m_concepts.end(); ++it) {
        const std::string& key = it->first;
        Concept* conceptPtr = it->second;

        std::cout << key << " -> " << std::endl;

        if (conceptPtr)
            conceptPtr->printRules();  // appelle la fonction de Concept
    }
}

void SemanticDescriptor::createBnf(const std::string& m_content) {
    std::vector<std::string> lines;
    std::istringstream iss(m_content);
    std::string line;

    while (std::getline(iss, line)) { // getline découpe sur '\n' par défaut
        lines.push_back(line);
        std::cout << line << std::endl;
    }


    size_t pos = parseConceptName(lines[19]);
    parseRule(lines[19], pos);



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




size_t SemanticDescriptor::parseRule(const std::string& m_content, size_t cursor) {

    std::string rule;

    size_t pos = m_content.find('\n');
    if (pos == std::string::npos)
        rule = m_content.substr(cursor);
    else
        rule = m_content.substr(cursor, pos);

    std::istringstream iss(rule);
    std::vector<std::string> parts;
    std::string word;

    ARule * arule = NULL; 

    while (iss >> word) {
        BnfMap map = ARule::getMap(word);
        arule = map.constructor();
        if (map.closer != "")
            iss >> word;
        while (word != map.closer)
        {
            if (word != DIVIDER)
            {
                arule->addItem(word);
                if (map.closer == "")
                    break;
            }
            if (!(iss >> word))
                break;
        }
        m_concepts[m_last_key]->setRule(arule);
      
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
    


    return pos;
}


size_t SemanticDescriptor::parseConceptName(const std::string& m_content) {
    size_t pos = m_content.find(EQRULE);
    if (pos == std::string::npos)
        throw std::runtime_error("Erreur: '::=' non trouvé");

    std::string before = m_content.substr(0, pos);

    before.erase(0, before.find_first_not_of(" \t\n\r"));
    before.erase(before.find_last_not_of(" \t\n\r") + 1);

    std::istringstream iss(before);
    std::vector<std::string> parts;
    std::string word;
    while (iss >> word) {
        parts.push_back(word);
    }

    if (parts.size() != 1) {
        throw std::runtime_error("Erreur: clé invalide (espaces internes détectés)");
    }

    std::cout << before << std::endl;

    m_concepts.insert(std::make_pair(before, new Concept()));
    m_last_key = before;

    return pos + EQRULE.length();
}
