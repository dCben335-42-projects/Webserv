#include <sstream>
#include <iostream>
#include "./Parser.hpp"
#include "Vector.hpp"
#include "Map.hpp"
#include "Logger.hpp"

Parser::Parser(const SemanticDescriptor &semantic) : m_semantic(semantic)
{
}

Parser::~Parser()
{
}

void Parser::createAllPath(std::string conceptName, PathSegment &currentPath, Vec<const PathSegment *> &allNodes)
{
	ConceptList conceptList = m_semantic.getConcept(conceptName);

	//     // }
}

// void printNode(ConceptNode *node)
// {
// 	std::cout << node->getName() << std::endl; // or whatever you want
// }

Vec<std::string> Parser::getFirstCharAccepted(ConceptNode *node) const
{
	// V_FOREACH(ConceptNode *, java, node->getChildren())
	// {
	// 	log(*java);
	// }

	while (node->hasChildren())
	{
		ConceptDef *concept = conceptList[i];
		Vec<ARule *> values = concept->getValues();

		if (concept->getIsTerminal())
		{
			for (size_t j = 0; j < values.size(); ++j)
			{
				Vec<std::string> items = values[j]->getItems();
				for (size_t k = 0; k < items.size(); ++k)
					allNodes.push(
						currentPath.addNextPathSegment(PathSegment(items[k], i, j, k, true)));
			}
			return;
		}
		for (size_t j = 0; j < values.size(); ++j)
		{
			Vec<std::string> items = values[j]->getItems();
			for (size_t k = 0; k < items.size(); ++k)
			{
				allNodes.push(
					currentPath.addNextPathSegment(PathSegment(items[k], i, j, k, false)));
				createAllPath(items[k], currentPath.getLastPathSegment(), allNodes);
			}
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

void Parser::detectPattern(ConceptNode *node, const char c)
{
	// log("Detecting pattern for word: " + c + " in concept: " + root->getName());

	// root->get

	ARule			*rule  = m_semantic.getConcepts()[node->getName()][0]->getValues()[0];
	Vec<std::string> items = rule->getItems();
	// items.has
}

void Parser::getAllTerminalChars(std::string conceptName, std::set<char> &allTerminalChars)
{
	if (startsWith(conceptName, "\'"))
	{
		allTerminalChars.insert(conceptName.begin() + 1, conceptName.end() - 1);
		return;
	}
	ConceptList conceptList = m_semantic.getConcept(conceptName);
	for (size_t i = 0; i < conceptList.size(); ++i)
	{
		ConceptDef *concept = conceptList[i];
		Vec<ARule *> values = concept->getValues();
		for (size_t j = 0; j < values.size(); ++j)
		{
			if (values[j]->getType() == OPTION)
			{
				Vec<std::string> items = values[j]->getItems();
				for (size_t k = 0; k < items.size(); ++k)
					getAllTerminalChars(items[k], allTerminalChars);
			}
			else if (values[j]->getType() == SYMBOL)
			{
				Vec<std::string> items = values[j]->getItems();
				getAllTerminalChars(items[0], allTerminalChars);
				break;
			}
			else
			{
				Vec<std::string> items = values[j]->getItems();
				for (size_t k = 0; k < items.size(); ++k)
					getAllTerminalChars(items[k], allTerminalChars);
				break;
			}
		}
	}
}

void Parser::recursive_parse(ConceptNode *root, const std::string &stringToParsed, size_t &charIndex)
{
	size_t childCount = root->getChildCount();
	if (childCount == 0)
	{
		detectPattern(root, stringToParsed.at(charIndex));
		charIndex++;
		return;
	}
	for (size_t i = 0; i < childCount; i++)
	{
		ConceptNode *child = root->getChild(i);

		log("Child " + std::to_string(i) + ": " + child->getName());

		recursive_parse(child, stringToParsed, charIndex);
		// void Parser::recursive_parse(char c, std::set<char> &currentSet, std::set<char> &nextSet)
		// {
		// 	(void) c;
		// 	(void) currentSet;
		// 	(void) nextSet;
		// }
	}
}
void Parser::recursive_parse(std::string conceptName, char c, std::set<char> &currentSet, std::set<char> &nextSet)
{
	// if (startsWith(conceptName, "\'"))
	// {
	// 	allTerminalChars.insert(conceptName.begin() + 1, conceptName.end() - 1);
	// 	return;
	// }
	ConceptList conceptDefList = m_semantic.getConcept(conceptName);

	for (size_t i = 0; i < conceptDefList.size(); ++i)
	{
		ConceptDef *concept = conceptDefList[i];
		Vec<ARule *> values = concept->getValues();

		for (size_t j = 0; j < values.size(); ++j) // trouver une soluce pour quil connaisse sa position ( par increment sur chaque concept encore valide )
		{
			if (values[j]->getType() == OPTION)
			{
				Vec<std::string> items = values[j]->getItems();
				return;
				for (size_t k = 0; k < items.size(); ++k)
					recursive_parse(items[k], c, currentSet, nextSet);
			}
			// else if (values[j]->getType() == SYMBOL)
			// {
			// 	Vec<std::string> items = values[j]->getItems();
			// 	getAllTerminalChars(items[0], allTerminalChars);
			// 	break;
			// }
			// else
			// {
			// 	Vec<std::string> items = values[j]->getItems();
			// 	for (size_t k = 0; k < items.size(); ++k)
			// 		getAllTerminalChars(items[k], allTerminalChars);
			// 	break;
			// }
		}
	}
}
void Parser::parse(const std::string &stringToParsed)
{

	if (stringToParsed.empty())
		return;

	size_t	wordsIndex	  = 0;
	size_t &refWordsIndex = wordsIndex;

	ConceptNode *root = m_semantic.getTree()->getChildren()[0];
	recursive_parse(root, stringToParsed, refWordsIndex);





	// std::cout << getFirstCharAccepted(m_semantic.getTree()->children[0]);

	log("\n------------------\n");
	log(getFirstCharAccepted(root));
	log("\n");


	// for (std::string::const_iterator it = stringToParsed.begin(); it != stringToParsed.end(); ++it)
	// {
	// 	char c = *it;

	// 	std::cout << c;
	// }

	// Map<std::string>  m_map;
	// Map<std::string> &m_map_ref		  = m_map;
	// PathSegment		  headsegment	  = PathSegment(CURSOR, 0, 0, 0, false);
	// PathSegment		 &headsegment_ref = headsegment;
	// Vec<const PathSegment *> allNodes;

	std::set<char> currentSet;
	std::set<char> nextSet;
	getAllTerminalChars(CURSOR, currentSet);


	log("------------------\n");
	printSet(currentSet);
	for (size_t i = 0; i < stringToParsed.size(); ++i)
	{
		char c = stringToParsed[i];
		if (currentSet.find(c) == currentSet.end())
		{
			log("Error: unexpected character '");
			log(std::string(1, c));
			log("'\n");
			return;
		}
		// createAllPath(CURSOR, headsegment_ref, allNodes);
		// recursive_parse(c, currentSet, nextSet);
		(void) c;
	}

	// log(headsegment_ref);
	(void) stringToParsed;
	(void) currentSet;
	(void) nextSet;
}