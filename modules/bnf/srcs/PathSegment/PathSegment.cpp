#include "PathSegment.hpp"

PathSegment::PathSegment(
	const std::string &conceptName,
	size_t			   conceptIndex,
	size_t			   position,
	size_t			   positionInSpecial,
	bool			   isTerminalConcept) :
	m_conceptName(conceptName),
	m_conceptIndex(conceptIndex),
	m_position(position),
	m_positionInSpecial(positionInSpecial),
	m_parent(NULL),
	m_depth(0),
	m_isTerminalConcept(isTerminalConcept)
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

const PathSegment *PathSegment::addNextPathSegment(const PathSegment &pathSegment)
{
	m_nextPathSegments.push_back(pathSegment);
	m_nextPathSegments.back().m_parent = this;
	m_nextPathSegments.back().setDepth(m_depth + 1);
	return &m_nextPathSegments.back();
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

PathSegment *PathSegment::getParent() const
{
	return m_parent;
}

void PathSegment::setDepth(size_t depth)
{
	m_depth = depth;
}

size_t PathSegment::getDepth() const
{
	return m_depth;
}

std::ostream &operator<<(std::ostream &os, const PathSegment &data)
{
	if (data.isTerminalConcept())
	{
		os << "TerminalTerm: " << data.getConceptName() << " at pos: " << data.getPositionInSpecial() << " depth: " << data.getDepth();
	}
	else
	{
		os << "PathSegment(conceptName: " << data.getConceptName()
		   << ", conceptIndex: " << data.getConceptIndex()
		   << ", position: " << data.getPosition()
		   << ", positionInSpecial: " << data.getPositionInSpecial()
		   << ", nextPathSegments: " << getLogJson(data.getNextPathSegments())
		   << ", depth: " << data.getDepth()
		   << ")";
	}
	return os;
}