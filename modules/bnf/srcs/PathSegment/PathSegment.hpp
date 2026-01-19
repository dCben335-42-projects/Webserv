#ifndef PATH_SEGMENT_HPP
#define PATH_SEGMENT_HPP

#include "Map.hpp"
#include "bnf_defs.hpp"

#include "../Rules/Group/Group.hpp"
#include "../Rules/Option/Option.hpp"
#include "../Rules/Sequence/Sequence.hpp"
#include "../Rules/Symbol/Symbol.hpp"
#include "../ConceptDef/ConceptDef.hpp"
#include "../ConceptNode/ConceptNode.hpp"
#include "../SemanticDescriptor/SemanticDescriptor.hpp"
#include "../utils/utils.hpp"
#include <set>

class PathSegment
{
	private:
		std::string		 m_conceptName;
		size_t			 m_conceptIndex;
		size_t			 m_position;
		size_t			 m_positionInSpecial;
		Vec<PathSegment> m_nextPathSegments;
		PathSegment		*m_parent;
		size_t			 m_depth;
		bool			 m_isTerminalConcept;

	public:
		PathSegment(const std::string &conceptName, size_t conceptIndex, size_t position, size_t positionInSpecial, bool isTerminalConcept);
		~PathSegment();
		const std::string	   &getConceptName() const;
		size_t					getConceptIndex() const;
		size_t					getPosition() const;
		size_t					getPositionInSpecial() const;
		const Vec<PathSegment> &getNextPathSegments() const;
		const PathSegment	   *addNextPathSegment(const PathSegment &pathSegment);
		PathSegment			   &getLastPathSegment();
		bool					isTerminalConcept() const;
		PathSegment			   *getParent() const;
		void					setDepth(size_t depth);
		size_t					getDepth() const;
};

std::ostream &operator<<(std::ostream &os, const PathSegment &data);

#endif