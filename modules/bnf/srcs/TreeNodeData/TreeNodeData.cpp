#include "./TreeNodeData.hpp"

TreeNodeData::TreeNodeData(std::string name, std::vector<Concept *> *concepts)
    : name(name), concepts(concepts) {}

bool TreeNodeData::is_multiple() const
{
    return (concepts->size() > 1);
}