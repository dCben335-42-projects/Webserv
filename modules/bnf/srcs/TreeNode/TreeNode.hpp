#ifndef TREENODE_HPP
#define TREENODE_HPP

#include "../TreeNodeData/TreeNodeData.hpp"

class TreeNode {
    public:
        TreeNode(TreeNodeData& data, std::vector<std::string> path);
        TreeNode(std::string value, std::vector<Concept *> *concepts);
        ~TreeNode() {};
        TreeNodeData &value;
        std::vector<std::string> path;
        std::vector<TreeNode *> children;
};

#endif