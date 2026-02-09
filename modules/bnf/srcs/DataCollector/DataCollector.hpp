
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

#include "../Exception/Parser/InvalidEntryException.hpp"
#include "../Exception/Parser/FileNotFinishedException.hpp"
#include "../BNFPath/BNFPath.hpp"
#include "Json.hpp"

class DataCollector
{
	public:
		DataCollector();
		~DataCollector();

		void generateJson(const BNFPath *cursor, const std::string &stringToParsed);
		void recurseJson(
			ARulePath				path,
			Vec<ARulePath>			pathNexts,
			size_t					index_rule,
			AJson				  **json,
			size_t				   *index,
			Vec<std::string>	   &line,
			std::string			   &buffer,
			Map<size_t, AJson *>   &mapIndexToJson,
			Vec<const ARule *>	   &collectStr,
			bool					hasCollectStr,
			bool				   &continueBuffer,
			Map<size_t, ARulePath> &pathStack);
};


#endif