
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
		int					   m_collectStrIdx;

	public:
		DataCollector();
		~DataCollector();

		void generateJson(const BNFPath *cursor, const std::string &strToParsed, Vec<std::string> lines);
		void recurseJson(
			size_t	index_rule,
			AJson **json,
			bool	hasCollectStr);

		bool			   samePrec(const size_t n) const;
		bool			   isEndOfPath() const;
		bool			   isSequence(const size_t idx) const;
		bool			   isLastRule(const size_t idx_rule) const;
		bool			   isTerminalConcept(const size_t idx_rule) const;
		AJson			  *getJson(const size_t idx);
		const std::string &getKey(const size_t idx) const;
		void			   delJson(const size_t idx);
		void			   movJson(AJson *json, const size_t idx);
		void			   addArrayIfNotExist(JsonArray *currentObject);
		void			   createNewObject(const size_t idx, const std::string &key);
		bool			   hasRegisteredPath(const size_t idx) const;
		void			   appendBuffer();
		JsonArray		  *getObjectArray(const size_t idx, const std::string &key);
		bool			   isEnd(size_t idx) const;
		int				   getLastSequence(Vec<ARulePath> pathStack, size_t idx) const;
		bool			   isEndP(ARulePath path, size_t idx) const;
		int				   getFirstTerminalConcept(ARulePath path) const;
		bool			   samePrecP(ARulePath path1, ARulePath path2, const size_t n) const;
		int				   getFirstSequence(ARulePath path, size_t max_bottom) const;
		bool			   findCharPos(
						  const std::vector<std::string> &lines,
						  size_t						  globalIndex,
						  size_t						 &lineIndex,
						  size_t						 &localIndex);
};


#endif