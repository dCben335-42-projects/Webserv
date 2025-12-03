#include "./TreeNode.hpp"
#include <iostream>
#include "../utils/utils.hpp"
#include "../Log/Log.hpp"

TreeNode::TreeNode(TreeNodeData& content, std::vector<std::string> path) : content(content), path(path) {}

TreeNode::TreeNode(std::string name, std::vector<Concept *> *concepts)
    : content(*(new TreeNodeData(name, concepts)))
{
    path.push_back(name);
}

std::ostream& operator<<(std::ostream& os, const TreeNode& data) {
    (void)os;
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