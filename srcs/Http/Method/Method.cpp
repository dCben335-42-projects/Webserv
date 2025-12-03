#include "Method.hpp"
#include <stdexcept>
#include "config.hpp"
#include "httpErrors.hpp"

static const MethodType g_methodsType[] =
{
    {.type = GET, .str = M_GET},
    {.type = POST, .str = M_POST},
    {.type = DELETE, .str = M_DELETE},
    {.type = PUT, .str = M_PUT},
    {.type = PATCH, .str = M_PATCH},
    {.type = NONE, .str = ""}
};
Method::Method(): m_type(NONE) {}

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
    throw Http501Error(method);
}

Method::Method(const EMethodType &method)
{
    MethodType m;
    for (size_t i = 0; g_methodsType[i].type != NONE; ++i)
    {
        m = g_methodsType[i];
        if (method == m.type)
        {
            m_type = m.type;
            m_strType = m.str;
            return ;
        }
    }
    throw Http501Error("adawdaw");
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

EMethodType Method::getType() const {
    return m_type;
}

std::string Method::getStrType() const {
    return m_strType;
}