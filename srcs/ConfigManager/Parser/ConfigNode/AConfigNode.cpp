#include "AConfigNode.hpp"


AConfigNode::AConfigNode()
{
}

AConfigNode::~AConfigNode()
{
}



const AConfigNode::Directives &AConfigNode::getDirectives() const
{
	return m_directives;
}
const Map<std::string> &AConfigNode::getAddHeaders() const
{
	return m_addHeaders;
}
const Map<std::string> &AConfigNode::getCgiPass() const
{
	return m_cgiPass;
}
const Map<std::string> &AConfigNode::getErrorPages() const
{
	return m_errorPages;
}


bool AConfigNode::hasDirective(const std::string &key) const
{
	return m_directives.has(key);
}

const Vec<std::string> &AConfigNode::getDirective(const std::string &key) const
{
	return m_directives.get(key);
}




void AConfigNode::setDirective(const std::string &key, const Vec<std::string> &value)
{
	m_directives.set(key, value);
}

void AConfigNode::setCgiPass(const std::string &key, const std::string &value)
{
	m_cgiPass.set(key, value);
}

void AConfigNode::setErrorPages(const std::string &key, const std::string &value)
{
	m_errorPages.set(key, value);
}

void AConfigNode::setAddHeaders(const std::string &key, const std::string &value)
{
	m_addHeaders.set(key, value);
}
