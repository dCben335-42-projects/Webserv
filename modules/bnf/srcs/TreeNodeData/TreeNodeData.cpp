#include "./TreeNodeData.hpp"

TreeNodeData::TreeNodeData(std::string name, ConceptList concepts)
    : name(name), concepts(concepts), isInfinite(true) {}

bool TreeNodeData::is_multiple() const
{
    return (concepts.size() > 1);
}

std::ostream& operator<<(std::ostream& os, const TreeNodeData& data)
{
    os << "TreeNodeData(name: " << data.name << ", concepts count: " 
       << (data.concepts.size()) << ")";
    return os;
}