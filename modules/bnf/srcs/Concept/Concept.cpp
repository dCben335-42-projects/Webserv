#include "Concept.hpp"
#include "Logger.hpp"

using namespace bnf;

bool hasAllCharsInMaj(std::string str)
{
	for (size_t i = 0; i < str.size(); ++i)
	{
		if (!std::isupper(str[i]) && !std::isdigit(str[i]) && str[i] != '_')
			return false;
	}
	return true;
}

Concept::Concept(const std::string conceptName) : m_conceptName(conceptName)
{
	if (hasAllCharsInMaj(conceptName))
		m_isTerminalConcept = true;
	else
		m_isTerminalConcept = false;
}

Concept::~Concept()
{
}

void Concept::pushDef(ConceptDef *definition)
{
	definitions.push_back(definition);
}

const Vec<ConceptDef *> &Concept::getDefinitions() const
{
	return definitions;
}

const std::string Concept::getConceptName() const
{
	return m_conceptName;
}

void Concept::addParent(const ARule *parent)
{
	m_parents.insert(parent);
}

const Set<const ARule *> &Concept::getParents() const
{
	return m_parents;
}

void Concept::addTerminalChar(const ARule *terminalChar)
{
	m_terminalChars.insert(terminalChar);
}

const Set<const ARule *> &Concept::getTerminalChars() const
{
	return m_terminalChars;
}

bool Concept::isTerminalConcept() const
{
	return m_isTerminalConcept;
}

std::ostream &operator<<(std::ostream &os, const Concept &data)
{
	os << "{\n";
	Logger::incrementIndent();
	os << Logger::indent() << "\"concept_name\": " << "\"" << data.getConceptName() << "\"\n";
	os << Logger::indent() << "\"is_terminal_concept\": " << (data.isTerminalConcept() ? "true" : "false") << "\n";
	os << Logger::indent() << "\"definitions\":";
	os << LoggerJson::get(data.getDefinitions());
	Logger::decrementIndent();
	os << "\n";

	Logger::incrementIndent();
	os << Logger::indent() << "\"parents\":";
	Logger::incrementIndent();
	os << LoggerJson::get(data.getParents(), true);
	Logger::decrementIndent();
	os << "\n";

	os << Logger::indent() << "\"terminal_chars\":";
	Logger::incrementIndent();
	os << LoggerJson::get(data.getTerminalChars(), true);
	Logger::decrementIndent();
	Logger::decrementIndent();
	os << "\n";
	os << Logger::indent() << "}";
	return os;
}