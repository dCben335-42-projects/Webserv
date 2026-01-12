#include "PathSegment.hpp"

PathSegment::PathSegment(const std::string &conceptName, size_t conceptIndex, size_t position, size_t positionInSpecial, bool isTerminalConcept) : m_conceptName(conceptName), m_conceptIndex(conceptIndex), m_position(position), m_positionInSpecial(positionInSpecial), m_isTerminalConcept(isTerminalConcept)
{
}

PathSegment::~PathSegment()
{
}

const std::string &PathSegment::getConceptName() const
{
	return m_conceptName;
}

size_t PathSegment::getConceptIndex() const
{
	return m_conceptIndex;
}

size_t PathSegment::getPosition() const
{
	return m_position;
}

size_t PathSegment::getPositionInSpecial() const
{
	return m_positionInSpecial;
}

const Vec<PathSegment> &PathSegment::getNextPathSegments() const
{
	return m_nextPathSegments;
}

void PathSegment::addNextPathSegment(const PathSegment &pathSegment)
{
	m_nextPathSegments.push_back(pathSegment);
}

PathSegment &PathSegment::getLastPathSegment()
{
	if (m_nextPathSegments.size() == 0)
		throw std::out_of_range("No next path segments available.");
	return m_nextPathSegments[m_nextPathSegments.size() - 1];
}

bool PathSegment::isTerminalConcept() const
{
	return m_isTerminalConcept;
}

std::ostream &operator<<(std::ostream &os, const PathSegment &data)
{
	if (data.isTerminalConcept())
	{
		os << "TerminalTerm: " << data.getConceptName() << " at pos: " << data.getPositionInSpecial();
	}
	else
	{
		os << "PathSegment(conceptName: " << data.getConceptName()
		   << ", conceptIndex: " << data.getConceptIndex()
		   << ", position: " << data.getPosition()
		   << ", positionInSpecial: " << data.getPositionInSpecial()
		   << ", nextPathSegments: " << getLogJson(data.getNextPathSegments())
		   << ")";
	}
	return os;
}