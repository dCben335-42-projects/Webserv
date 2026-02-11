
#ifndef DATA_COLLECTOR_HPP
#define DATA_COLLECTOR_HPP

#include "Map.hpp"
#include "bnf_defs.hpp"

#include "../Rules/ARule.hpp"
#include "../Rules/Group/Group.hpp"
#include "../Rules/Option/Option.hpp"
#include "../Rules/Sequence/Sequence.hpp"
#include "../Rules/Symbol/Symbol.hpp"
#include "../Concept/ConceptDef/ConceptDef.hpp"
#include "../Concept/ConceptNode/ConceptNode.hpp"
#include "../SemanticDescriptor/SemanticDescriptor.hpp"
#include "../utils/utils.hpp"

#include "../BNFPath/BNFPath.hpp"
#include "Json.hpp"

class DataCollector
{
	private:
		std::string			   m_buffer;
		size_t				   m_cidx;
		Vec<ARulePath>		   m_pathNexts;
		ARulePath			   m_path;
		Map<size_t, ARulePath> m_pathStack;
		Map<size_t, AJson *>   m_mapJson;
		bool				   m_continueBuffer;
		Vec<std::string>	   m_lines;
		Vec<const ARule *>	   m_collectStr;

	public:
		DataCollector();
		~DataCollector();

		void generateJson(const BNFPath *cursor, const std::string &stringToParsed);
		void recurseJson(
			size_t	index_rule,
			AJson **json,
			bool	hasCollectStr);

		bool			   same_prec(const size_t n) const;
		bool			   isEndOfPath() const;
		bool			   isSequence(size_t idx) const;
		bool			   isLastRule(size_t idx_rule) const;
		bool			   isTerminalConcept(size_t idx_rule) const;
		AJson			  *getJson(size_t idx);
		const std::string &getKey(size_t idx) const;
		void			   delJson(const size_t idx);
		void			   movJson(AJson *json, const size_t idx);
		void			   addArrayIfNotExist(JsonArray *currentObject);
};


#endif