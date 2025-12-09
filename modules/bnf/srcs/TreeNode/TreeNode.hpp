#ifndef TREENODE_HPP
#define TREENODE_HPP

#include "../TreeNodeData/TreeNodeData.hpp"

class TreeNode {
    public:
        TreeNode(TreeNodeData& content, std::vector<std::string> path);
        // TreeNode(std::string name, ConceptList concepts);
        // createTree();
        ~TreeNode();
        TreeNodeData &content;
        std::vector<std::string> path;
        std::vector<TreeNode *> children;
};

std::ostream& operator<<(std::ostream& os, const TreeNode& data);

#endif