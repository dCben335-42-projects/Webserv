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

Vec<std::string> Parser::getTerminalChars(ConceptNode *node)
{
	ConceptList RulesConcept = m_semantic.getConcepts()[node->getName()];
	return RulesConcept[0]->getValues()[0]->getItems();
}

void Parser::createAllPath(std::string conceptName, PathSegment &currentPath)
{
	ConceptList conceptList = m_semantic.getConcepts()[conceptName];
	if (conceptList.empty())
		return;

	for (size_t i = 0; i < conceptList.size(); ++i)
	{
		Concept *concept  = conceptList[i];
		bool terminalTerm = false;

		if (concept->getIsTerminal())
		{
			terminalTerm = true;
		}
		Vec<ARule *> values = concept->getValues();
		for (size_t j = 0; j < values.size(); ++j)
		{
			Vec<std::string> items = values[j]->getItems();
			if (values[j]->getType() == SYMBOL)
			{
				currentPath.addNextPathSegment(PathSegment(items[0], i, j, 0, terminalTerm));
				createAllPath(items[0], currentPath.getLastPathSegment());
			}
			else if (values[j]->getType() == SEQUENCE)
			{
				for (size_t k = 0; k < items.size(); ++k)
				{
					currentPath.addNextPathSegment(PathSegment(items[k], i, j, k, terminalTerm));
					createAllPath(items[k], currentPath.getLastPathSegment());
				}
			}
			else if (values[j]->getType() == OPTION)
			{
				for (size_t k = 0; k < items.size(); ++k)
				{
					currentPath.addNextPathSegment(PathSegment(items[k], i, j, k, terminalTerm));
					createAllPath(items[k], currentPath.getLastPathSegment());
				}
			}
			else if (values[j]->getType() == GROUP)
			{
				for (size_t k = 0; k < items.size(); ++k)
				{
					currentPath.addNextPathSegment(PathSegment(items[k], i, j, k, terminalTerm));
					createAllPath(items[k], currentPath.getLastPathSegment());
				}
			}
		}
	}
}

void Parser::recursive_parse(PathSegment &currentPath, char c, Map<std::string> &m_map_ref)
{
	if (startsWith(conceptName, "\'"))
	{
		log("Terminal found: " + conceptName + "\n");
	}
	ConceptList conceptList = m_semantic.getConcept(conceptName);

	if (conceptList.empty())
		return;
	for (size_t i = 0; i < conceptList.size(); ++i)
	{
		Concept *concept	= conceptList[i];
		Vec<ARule *> values = concept->getValues();
		for (size_t j = 0; j < values.size(); ++j)
		{
			Vec<std::string> items = values[j]->getItems();
			if (values[j]->getType() == SYMBOL)
			{
				recursive_parse(items[0], c, m_map_ref);
			}
			else if (values[j]->getType() == SEQUENCE)
			{
				for (size_t k = 0; k < items.size(); ++k)
				{
					recursive_parse(items[k], c, m_map_ref);
				}
			}
			else if (values[j]->getType() == OPTION)
			{
				for (size_t k = 0; k < items.size(); ++k)
				{
					recursive_parse(items[k], c, m_map_ref);
				}
			}
			else if (values[j]->getType() == GROUP)
			{
				for (size_t k = 0; k < items.size(); ++k)
				{
					recursive_parse(items[k], c, m_map_ref);
				}
			}
		}
	}
}

void Parser::parse(const std::string &stringToParsed)
{
	if (stringToParsed.empty())
		return;

	Map<std::string>  m_map;
	Map<std::string> &m_map_ref		  = m_map;
	PathSegment		  headsegment	  = PathSegment(CURSOR, 0, 0, 0, false);
	PathSegment		 &headsegment_ref = headsegment;

	log("------------------\n");
	for (size_t i = 0; i < stringToParsed.size(); ++i)
	{
		char c = stringToParsed[i];
		createAllPath(CURSOR, headsegment_ref);
		recursive_parse(headsegment_ref, c, m_map_ref);
	}

	log(headsegment_ref);
	(void) stringToParsed;
}