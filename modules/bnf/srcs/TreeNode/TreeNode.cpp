#include "./TreeNode.hpp"
#include <iostream>
#include "../utils/utils.hpp"
#include "../Log/Log.hpp"

TreeNode::TreeNode(TreeNodeData& content, std::vector<std::string> path) : content(content), path(path) {}

// TreeNode TreeNode::createTree()
// {
//     Vec<std::string>::type path;
//     TreeNodeData data(CURSOR, m_concepts[CURSOR]);
//     path.push_back(CURSOR);
//     tree = new TreeNode(data, path);
// }

TreeNode::~TreeNode() {
    for (Vec<TreeNode *>::iterator it = children.begin(); it != children.end(); ++it) {
        delete (*it);
    }
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