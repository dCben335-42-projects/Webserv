#include "./TreeNodeData.hpp"

TreeNodeData::TreeNodeData(std::string value, std::vector<Concept *> *concepts)
    : value(value), concepts(concepts) {}

bool TreeNodeData::is_multiple() const
{
    return (concepts->size() > 1);
}