#include "Concept.hpp"
#include "Logger.hpp"

using namespace bnf;

Concept::Concept(const std::string conceptName) : m_conceptName(conceptName)
{
}

Concept::~Concept()
{
}

void Concept::pushDef(ConceptDef *definition)
{
	definitions.push_back(definition);
}

Vec<ConceptDef *> &Concept::getDefinitions()
{
	return definitions;
}

const Vec<ConceptDef *> &Concept::getDefinitions() const
{
	return definitions;
}

const std::string Concept::getConceptName() const
{
	return m_conceptName;
}

void Concept::addParent(ARule *parent)
{
	m_parents.insert(parent);
}

const std::set<ARule *> &Concept::getParents() const
{
	return m_parents;
}

void Concept::addTerminalChar(ARule *terminalChar)
{
	m_terminalChars.insert(terminalChar);
}

const std::set<ARule *> &Concept::getTerminalChars() const
{
	return m_terminalChars;
}

std::ostream &operator<<(std::ostream &os, const Concept &data)
{
	os << Log::indent() << "{\n";
	Log::incrementIndent();
	os << Log::indent() << "Concept Name: " << data.getConceptName() << "\n"
	   << Log::indent() << "Definitions:\n";
	log(data.getDefinitions());
	Log::decrementIndent();
	os << "\n"
	   << Log::indent() << "Parents: ";
	printSet(data.getParents());
	os << Log::indent() << "Terminal Chars: ";
	printSet(data.getTerminalChars());
	os << Log::indent() << "}";
	return os;
}