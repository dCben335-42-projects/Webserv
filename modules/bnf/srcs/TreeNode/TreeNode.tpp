#include "./TreeNode.hpp"
#include "Logger.hpp"
#include <iostream>

template <typename Derived>
TreeNode<Derived>::TreeNode() : m_parent(NULL)
{
}

template <typename Derived>
TreeNode<Derived>::~TreeNode()
{
	for (typename Vec<Derived *>::iterator it = m_children.begin(); it != m_children.end(); ++it)
	{
		delete (*it);
	}
}

template <typename Derived>
Derived &TreeNode<Derived>::setParent(TreeNode<Derived> *parentPtr)
{
	m_parent = parentPtr;
	if (parentPtr)
		parentPtr->m_children.push_back(static_cast<Derived *>(this));
	return (static_cast<Derived &>(*this));
}

template <typename Derived>
Derived &TreeNode<Derived>::addSegmentPath(std::string segment)
{
	m_path.push_back(segment);
	return (static_cast<Derived &>(*this));
}

template <typename Derived>
Derived &TreeNode<Derived>::setPath(Vec<std::string> newPath)
{
	m_path = newPath;
	return (static_cast<Derived &>(*this));
}

template <typename Derived>
std::ostream &operator<<(std::ostream &os, const TreeNode<Derived> &data)
{
	Logger::incrementIndent();
	std::cout << "\n";
	for (size_t i = 0; i < data.getChildren().size(); i++)
	{
		std::cout << Logger::indent();
		log(data.getChildren()[i]);
	}
	Logger::decrementIndent();
	return os;
}

template <typename Derived>
TreeNode<Derived> *TreeNode<Derived>::getParent() const
{
	return m_parent;
}

template <typename Derived>
const Vec<Derived *> &TreeNode<Derived>::getChildren() const
{
	return m_children;
}

template <typename Derived>
const Vec<std::string> &TreeNode<Derived>::getPath() const
{
	return m_path;
}

template <typename Derived>
Derived *TreeNode<Derived>::getChild(size_t idx) const
{
	if (m_children.size() > idx)
		return m_children[idx];
	return NULL;
}

template <typename Derived>
size_t TreeNode<Derived>::getChildCount() const
{
	return m_children.size();
}

template <typename Derived>
bool TreeNode<Derived>::hasChildren() const
{
	return !m_children.empty();
}