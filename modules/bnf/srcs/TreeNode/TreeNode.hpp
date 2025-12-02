#ifndef TREENODE_HPP
#define TREENODE_HPP

#include "../TreeNodeData/TreeNodeData.hpp"

class TreeNode {
    public:
        TreeNode(TreeNodeData& content, std::vector<std::string> path);
        TreeNode(std::string name, std::vector<Concept *> *concepts);
        ~TreeNode() {};
        TreeNodeData &content;
        std::vector<std::string> path;
        std::vector<TreeNode *> children;
};

#endif