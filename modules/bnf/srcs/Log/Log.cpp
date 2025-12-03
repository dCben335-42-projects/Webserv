#include "Log.hpp"
#include <cstddef>
#include <iostream>

size_t Log::m_indent = 0;

size_t Log::incrementIndent() {
    return ++m_indent;
}

size_t Log::decrementIndent() {
    if (m_indent > 0)
        --m_indent;
    return m_indent;
}

size_t Log::getIndent() {
    return m_indent;
}

std::string Log::indent() {
    size_t indent = Log::getIndent();
    return std::string(indent * 4, ' ');
}