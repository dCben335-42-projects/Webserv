#include "./TreeNode.hpp"
#include "Logger.hpp"
#include <iostream>

TreeNode::TreeNode() :
	m_parent(NULL)
{
}

TreeNode::~TreeNode()
{
	for (Vec<TreeNode *>::iterator it = m_children.begin(); it != m_children.end(); ++it)
	{
		delete (*it);
	}
}

TreeNode &TreeNode::setParent(TreeNode *parentPtr)
{
	m_parent = parentPtr;
	parentPtr->m_children.push_back(this);
	return (*this);
}

TreeNode &TreeNode::addSegmentPath(std::string segment)
{
	m_path.push_back(segment);
	return (*this);
}

TreeNode &TreeNode::setPath(Vec<std::string> newPath)
{
	m_path = newPath;
	return (*this);
}

std::ostream &operator<<(std::ostream &os, const TreeNode &data)
{
	// log(data.);
	Log::incrementIndent();
	std::cout << "\n";
	for (size_t i = 0; i < data.getChildren().size(); i++)
	{
		std::cout << Log::indent();
		log(data.getChildren()[i]);
	}
	Log::decrementIndent();

	return os;
}

TreeNode *TreeNode::getParent() const
{
	return m_parent;
}

Vec<TreeNode *> TreeNode::getChildren() const
{
	return m_children;
}

Vec<std::string> TreeNode::getPath() const
{
	return m_path;
}