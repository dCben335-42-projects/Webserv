#include "../includes/Logger.hpp"

size_t		   Logger::m_indent = 0;
Logger::Target Logger::m_target = Logger::Stdout;
std::ofstream  Logger::m_file;

void Logger::setTarget(Target t)
{
	m_target = t;
}

void Logger::setFile(const std::string &path)
{
	m_file.open(path.c_str(), std::ios::app);
	m_target = File;
}

std::ostream &Logger::stream()
{
	switch (m_target)
	{
		case Stderr:
			return std::cerr;
		case File:
			return m_file;
		default:
			return std::cout;
	}
}

void Logger::log(const std::string &str)
{
	std::cerr << "String:" << std::endl;
	stream() << indent() << str << std::endl;
}
void Logger::log(const char *cstr)
{
	std::cerr << "C-String:" << std::endl;
	stream() << indent() << cstr << std::endl;
}
void Logger::log(char c)
{
	std::cerr << "Char:" << std::endl;
	stream() << indent() << c << std::endl;
}


size_t Logger::incrementIndent()
{
	return ++m_indent;
}
size_t Logger::decrementIndent()
{
	if (m_indent > 0)
		--m_indent;
	return m_indent;
}
size_t Logger::getIndent()
{
	return m_indent;
}
std::string Logger::indent()
{
	return std::string(m_indent * INDENT_SIZE, INDENT_CHAR);
}
