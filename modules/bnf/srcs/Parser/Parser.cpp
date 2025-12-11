#include <sstream>
#include <iostream>
#include "./Parser.hpp"
#include "Vector.hpp"
#include "Logger.hpp"

Parser::Parser(const SemanticDescriptor& semantic) : m_semantic(semantic) {}

Parser::~Parser() {}


// Vec<char>::type Parser::getCharAccepted(TreeNode *node) const
// {
//     Vec<TreeNode *>::type children = node->children;
//     // for (Vec<TreeNode *>::iterator it = children.begin(); it != children.end(); ++it) {
        
//     // }
// }

// std::string Parser::getFirstCharAccepted(TreeNode *node) const
// {
//     while (node->children.size() > 0)
//     {
//         node = node->children[0];
//         if (node->children.size() > 0)
//             continue;
//         else
//         {

//         }
//     }
//     Vec<char>::type eventualChar;
//     // ARule *rule = m_semantic.getConcepts()[node->content.name][0]->getValues()[0];
//     // log(m_semantic.getConcepts()[node->content.name][0]->getValues()[0]->getItems());
    

//     // if (rule->getType() == GROUP)
//     // {
//     //     Vec<std::string>::type items = rule->getItems();
//     // }

//     // m_semantic.getConcepts()[node->content.name][0]->getValues();
//     return (node->content);
//     // for (Vec<TreeNode *>::iterator it = children.begin(); it != children.end(); ++it) {
        
//     // }
// }


// Vec<char>::type Parser::getCharAcceptedHead() const
// {
//     // Vec<TreeNode *>::type children = m_semantic.getTree()->children;
//     // for (Vec<TreeNode *>::iterator it = children.begin(); it != children.end(); ++it) {
//     //     TreeNode * val =        *it;
//     // }
// }

void Parser::parse(const std::string& stringToParsed) {

    // std::cout << getFirstCharAccepted(m_semantic.getTree()->children[0]);
    // for (std::string::const_iterator it = stringToParsed.begin(); it != stringToParsed.end(); ++it) {
        //     char c = *it;
        
        //     std::cout << c;
        // }
    (void)stringToParsed;
}