#include "Upload.hpp"

Upload::Upload() : m_isOn(false), m_uploadDir("")
{
}
Upload::Upload(const Upload &other) : m_isOn(other.m_isOn), m_uploadDir(other.m_uploadDir)
{
}

const std::string &Upload::getUploadDir() const
{
	return m_uploadDir;
}
bool Upload::isOn() const
{
	return m_isOn;
}

void Upload::setUploadDir(const std::string &uploadDir)
{
	m_uploadDir = uploadDir;
}
void Upload::setOn(bool isOn)
{
	m_isOn = isOn;
}