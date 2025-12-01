#include "./TreeNode.hpp"

TreeNode::TreeNode(TreeNodeData& data, std::vector<std::string> path) : value(data), path(path) {}

TreeNode::TreeNode(std::string value, std::vector<Concept *> *concepts)
    : value(*(new TreeNodeData(value, concepts))) {}