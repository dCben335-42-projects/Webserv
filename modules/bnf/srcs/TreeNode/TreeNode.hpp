#ifndef TREENODE_HPP
#define TREENODE_HPP

#include "Vector.hpp"
#include <string>

class TreeNode
{
	protected:
		TreeNode		*m_parent;
		Vec<TreeNode *>	 m_children;
		Vec<std::string> m_path;

	public:
		TreeNode();
		virtual ~TreeNode();

		TreeNode &setParent(TreeNode *parentPtr);
		TreeNode &setPath(Vec<std::string> path);
		TreeNode &addSegmentPath(std::string segment);

		TreeNode		*getParent() const;
		Vec<std::string> getPath() const;
		Vec<TreeNode *>	 getChildren() const;
};

std::ostream &operator<<(std::ostream &os, const TreeNode &data);

#endif
