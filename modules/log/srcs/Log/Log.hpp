#ifndef LOG_HPP
#define LOG_HPP

#include <cstddef>
#include <string>

# define SPACE		' '
# define TAP_SIZE	4

class Log
{
    private:
        static size_t m_indent;
    public:
        static size_t incrementIndent();
        static size_t decrementIndent();
        static size_t getIndent();
        static std::string indent();
};

#endif