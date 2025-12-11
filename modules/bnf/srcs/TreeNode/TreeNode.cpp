#include "./TreeNode.hpp"
#include "Logger.hpp"
#include <iostream>

TreeNode::TreeNode(TreeNodeData &content, std::vector<std::string> path):
    content(content),
    path(path)
{}

TreeNode::~TreeNode() {
    for (Vec<TreeNode *>::iterator it = children.begin(); it != children.end(); ++it) {
        delete (*it);
    }
}

std::ostream &operator<<(std::ostream &os, const TreeNode &data)
{
    log(data.content);
    Log::incrementIndent();
    std::cout << "\n";
    for (size_t i = 0; i < data.children.size(); i++)
    {
        std::cout << Log::indent();
        log(data.children[i]);
    }
    Log::decrementIndent();

    return os;
}