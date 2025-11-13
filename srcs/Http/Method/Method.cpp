#include "Method.hpp"
#include <stdexcept>
#include "config.hpp"

static const MethodType g_methodsType[] =
{
    {.type = GET, .str = M_GET},
    {.type = POST, .str = M_POST},
    {.type = DELETE, .str = M_DELETE},
    {.type = PUT, .str = M_PUT},
    {.type = PATCH, .str = M_PATCH},
    {.type = NONE, .str = ""}
};

Method::Method(const std::string &method)
{
    MethodType m;
    for (size_t i = 0; g_methodsType[i].type != NONE; ++i)
    {
        m = g_methodsType[i];
        if (method == m.str)
        {
            m_type = m.type;
            m_strType = m.str;
            return ;
        }
    }
    throw std::invalid_argument("Invalid Method" + method);
}

Method &Method::operator=(const Method &other)
{
    if (this == &other)
        return *this;
    m_type = other.m_type;
    m_strType = other.m_strType;
    return *this;
}

Method::~Method(){}

EMethodType &Method::getType() {
    return m_type;
}

std::string &Method::getStrType() {
    return m_strType;
}