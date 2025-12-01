#ifndef TREENODE_HPP
#define TREENODE_HPP

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

class TreeNode
{
    public:
        std::string value;
        std::vector<Concept *> *concepts;
        std::vector<TreeNode *> children;
        std::vector<std::string> path;
        std::vector<ESecure> recurse_safe;
        bool is_multiple() const;
};

#endif