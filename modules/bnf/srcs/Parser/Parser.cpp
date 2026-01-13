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

	for (size_t i = 0; i < conceptList.size(); ++i)
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
}

// void Parser::recursive_parse(char c, std::set<char> &currentSet, std::set<char> &nextSet)
// {
// 	(void) c;
// 	(void) currentSet;
// 	(void) nextSet;
// }

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