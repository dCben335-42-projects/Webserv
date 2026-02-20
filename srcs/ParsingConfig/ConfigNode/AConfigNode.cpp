#include "AConfigNode.hpp"


AConfigNode::AConfigNode()
{
}

AConfigNode::~AConfigNode()
{
}

// ===== GETTERS =====
const AConfigNode::Directives &AConfigNode::getDirectives() const
{
	return directives;
}
const Map<std::string> &AConfigNode::getAddHeaders() const
{
	return add_headers;
}
const Map<std::string> &AConfigNode::getCgiPass() const
{
	return cgi_pass;
}
const Map<std::string> &AConfigNode::getErrorPages() const
{
	return error_pages;
}

// ===== SETTERS =====
void AConfigNode::setDirectiveAt(const std::string &key, const Vec<std::string> &value)
{
	directives[key] = value;
}

void AConfigNode::setCgiPassAt(const std::string &key, const std::string &value)
{
	cgi_pass[key] = value;
}

void AConfigNode::setErrorPagesAt(const std::string &key, const std::string &value)
{
	error_pages[key] = value;
}

void AConfigNode::setAddHeadersAt(const std::string &key, const std::string &value)
{
	add_headers[key] = value;
}