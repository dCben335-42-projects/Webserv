#include "./ConceptNode.hpp"

ConceptNode::ConceptNode(const std::string name, const ConceptList &concepts) : m_name(name), m_concepts(concepts), m_hasInfRecursivity(true)
{
}

bool ConceptNode::is_multiple() const
{
	return (m_concepts.size() > 1);
}

std::ostream &operator<<(std::ostream &os, const ConceptNode &data)
{
	os << "ConceptNode(name: " << data.getName() << ", concepts count: "
	   << (data.getConcepts().size()) << ")";
	return os;
}

const ConceptList &ConceptNode::getConcepts() const
{
	return m_concepts;
}

const std::string &ConceptNode::getName() const
{
	return m_name;
}

bool ConceptNode::hasInfRecursivity() const
{
	return m_hasInfRecursivity;
}

void ConceptNode::setHasInfRecursivity(const bool hasInfRecursivity)
{
	m_hasInfRecursivity = hasInfRecursivity;
}