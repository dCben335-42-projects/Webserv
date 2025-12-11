#include "./TreeNode.hpp"
#include "Logger.hpp"
#include <iostream>

TreeNode::TreeNode(TreeNodeData &content):
    content(content),
    parent(NULL)
{}

TreeNode::~TreeNode() {
    for (Vec<TreeNode *>::iterator it = children.begin(); it != children.end(); ++it) {
        delete (*it);
    }
}

TreeNode& TreeNode::setParent(TreeNode *parentPtr)
{
    parent = parentPtr;
    parentPtr->children.push_back(this);
    return (*this);
}

TreeNode& TreeNode::addSegmentPath(std::string segment)
{
    path.push_back(segment);
    return (*this);
}

TreeNode& TreeNode::setPath(Vec<std::string>::type newPath)
{
    path = newPath;
    return (*this);
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