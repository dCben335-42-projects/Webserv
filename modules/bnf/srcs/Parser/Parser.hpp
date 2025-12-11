#ifndef PARSER_HPP
#define PARSER_HPP

#include "Map.hpp"
#include "bnf_defs.hpp"

#include "../Rules/Group/Group.hpp"
#include "../Rules/Option/Option.hpp"
#include "../Rules/Sequence/Sequence.hpp"
#include "../Rules/Symbol/Symbol.hpp"
#include "../Concept/Concept.hpp"
#include "../TreeNode/TreeNode.hpp"
#include "../SemanticDescriptor/SemanticDescriptor.hpp"
#include "../utils/utils.hpp"

class Parser {
    private:
        const SemanticDescriptor& m_semantic;
        Vec<char>::type getCharAcceptedHead() const;
        Vec<char>::type getCharAccepted(TreeNode *node) const;
        std::string getFirstCharAccepted(TreeNode *node) const;
    public:
        ~Parser();
        Parser(const SemanticDescriptor& semantic);
        void parse(const std::string& stringToParsed);
};

// std::ostream& operator<<(std::ostream& os, const SemanticDescriptor& data);

#endif