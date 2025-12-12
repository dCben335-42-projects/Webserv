#include <sstream>
#include <iostream>
#include "./Parser.hpp"
#include "Vector.hpp"
#include "Logger.hpp"

Parser::Parser(const SemanticDescriptor &semantic) : m_semantic(semantic)
{
}

Parser::~Parser()
{
}


// Vec<char> Parser::getCharAccepted(TreeNode *node) const
// {
//     Vec<TreeNode *> children = node->children;
//     // for (Vec<TreeNode *>::iterator it = children.begin(); it != children.end(); ++it) {

//     // }
// }

Vec<std::string> Parser::getFirstCharAccepted(ConceptNode *node) const
{
	while (node->getChildren().size() > 0)
	{
		node = node->getChildren()[0];
		if (node->getChildren().size() > 0)
			continue;
		else
		{
		}
	}
	Vec<char> eventualChar;
	ARule	 *rule = m_semantic.getConcepts()[node->getName()][0]->getValues()[0];
	// log(m_semantic.getConcepts());

	// log(m_semantic.getConcepts()[node->content.name]);

	// log(m_semantic.getConcepts()[node->content.name][0]);

	// log(m_semantic.getConcepts()[node->content.name][0]->getValues());

	// log(m_semantic.getConcepts()[node->content.name][0]->getValues()[0]);

	// log(m_semantic.getConcepts()[node->content.name][0]->getValues()[0]->getItems());

	// if (rule->getType() == OPTION)
	// {

	// }



	// if (rule->getType() == GROUP)
	// {
	// Vec<std::string> items = rule->getItems();
	// }

	// m_semantic.getConcepts()[node->content.name][0]->getValues();
	// return (node->content);
	// for (Vec<TreeNode *>::iterator it = children.begin(); it != children.end(); ++it) {

	// }
	return (rule->getItems());
}


// Vec<char> Parser::getCharAcceptedHead() const
// {
//     // Vec<TreeNode *> children = m_semantic.getTree()->children;
//     // for (Vec<TreeNode *>::iterator it = children.begin(); it != children.end(); ++it) {
//     //     TreeNode * val =        *it;
//     // }
// }

void Parser::parse(const std::string &stringToParsed)
{

	// std::cout << getFirstCharAccepted(m_semantic.getTree()->children[0]);

	log("\n------------------\n");
	log(getFirstCharAccepted(m_semantic.getTree()->getChildren()[0]));
	log("\n");


	// for (std::string::const_iterator it = stringToParsed.begin(); it != stringToParsed.end(); ++it)
	// {
	// 	char c = *it;

	// 	std::cout << c;
	// }
	(void) stringToParsed;
}