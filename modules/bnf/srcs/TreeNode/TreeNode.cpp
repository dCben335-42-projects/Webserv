#include "./TreeNode.hpp"
#include <iostream>

TreeNode::TreeNode(TreeNodeData& content, std::vector<std::string> path) : content(content), path(path) {
    // std::cout << content << std::endl;
}

TreeNode::TreeNode(std::string name, std::vector<Concept *> *concepts)
    : content(*(new TreeNodeData(name, concepts))) {}