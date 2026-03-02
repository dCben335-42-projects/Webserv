#include "Upload.hpp"

Upload::Upload() : m_isOn(false), m_uploadDir("")
{
}
Upload::Upload(const Upload &other) : m_isOn(other.m_isOn), m_uploadDir(other.m_uploadDir)
{
}

Upload &Upload::operator=(const Upload &other)
{
	if (this != &other)
	{
		m_isOn		= other.m_isOn;
		m_uploadDir = other.m_uploadDir;
	}
	return *this;
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

#include "ostream"

std::ostream &operator<<(std::ostream &os, const Upload &upload)
{
	os << "Upload: " << (upload.isOn() ? "on" : "off") << ", upload dir: " << upload.getUploadDir();
	return os;
}