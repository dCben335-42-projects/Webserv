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
	os << Log::indent() << "}";
	return os;
}