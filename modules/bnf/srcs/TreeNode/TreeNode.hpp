#ifndef TREENODE_HPP
#define TREENODE_HPP

#include "Vector.hpp"
#include <string>

template <typename Derived>
class TreeNode
{
	protected:
		TreeNode<Derived> *m_parent;
		Vec<Derived *>	   m_children;
		Vec<std::string>   m_path;

	public:
		TreeNode();
		virtual ~TreeNode();

		TreeNode<Derived> &setParent(TreeNode<Derived> *parentPtr);
		TreeNode<Derived> &setPath(Vec<std::string> path);
		TreeNode<Derived> &addSegmentPath(std::string segment);

		TreeNode<Derived>	   *getParent() const;
		const Vec<std::string> &getPath() const;
		const Vec<Derived *>   &getChildren() const;
};

template <typename Derived>
std::ostream &operator<<(std::ostream &os, const TreeNode<Derived> &data);

#include "./TreeNode.tpp"

#endif
