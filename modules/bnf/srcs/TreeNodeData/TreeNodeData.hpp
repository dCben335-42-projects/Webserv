#ifndef TREENODEDATA_HPP
#define TREENODEDATA_HPP

#include <map>
#include <vector>
#include "bnf_defs.hpp"
#include "../Rules/ARule.hpp"
#include "../Rules/Group/Group.hpp"
#include "../Rules/Option/Option.hpp"
#include "../Rules/Sequence/Sequence.hpp"
#include "../Rules/Symbol/Symbol.hpp"
#include "../Concept/Concept.hpp"

enum ESecure {
    SECURE,
    NOT_SECURE
};

class TreeNodeData
{
    public:
        TreeNodeData(std::string value, std::vector<Concept *> *concepts);
        ~TreeNodeData() {};
        std::string value;
        std::vector<Concept *> *concepts;
        bool is_multiple() const;
};

#endif