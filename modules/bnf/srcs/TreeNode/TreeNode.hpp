#ifndef TREENODE_HPP
#define TREENODE_HPP

#include "../TreeNodeData/TreeNodeData.hpp"

class TreeNode
{
	public:
		TreeNode(TreeNodeData &content);
		~TreeNode();
		TreeNodeData			&content;
		std::vector<std::string> path;
		std::vector<TreeNode *>	 children;
		TreeNode				&setParent(TreeNode *parentPtr);
		TreeNode				&addSegmentPath(std::string segment);
		TreeNode				&setPath(Vec<std::string> path);
		TreeNode				*parent;
};

std::ostream &operator<<(std::ostream &os, const TreeNode &data);

#endif