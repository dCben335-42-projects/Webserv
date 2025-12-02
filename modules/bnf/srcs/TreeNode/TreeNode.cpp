#include "./TreeNode.hpp"

TreeNode::TreeNode(TreeNodeData& content, std::vector<std::string> path) : content(content), path(path) {}

TreeNode::TreeNode(std::string name, std::vector<Concept *> *concepts)
    : content(*(new TreeNodeData(name, concepts))) {}