#include "./TreeNodeData.hpp"

TreeNodeData::TreeNodeData(std::string name, std::vector<Concept *> *concepts)
    : name(name), concepts(concepts), isEnded(false) {}

bool TreeNodeData::is_multiple() const
{
    return (concepts->size() > 1);
}

std::ostream& operator<<(std::ostream& os, const TreeNodeData& data)
{
    os << "TreeNodeData(name: " << data.name << ", concepts count: " 
       << (data.concepts ? data.concepts->size() : 0) << ")";
    return os;
}