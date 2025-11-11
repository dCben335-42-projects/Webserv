#ifndef URL_TPP
#define URL_TPP

#include "Url.hpp"
#include <stdexcept>

template <typename Scheme>
Url<Scheme>::Url(const std::string& url) :
    AUrl(url),
    m_scheme(Scheme::scheme),
    m_schemePattern(Scheme::pattern),
    m_schemeName(Scheme::name)
{
    if (!isValid())
        throw std::invalid_argument("Invalid URL for scheme");
}

template <typename Scheme>
std::string Url<Scheme>::getSchemePattern() const {
    return m_schemePattern;
}

template <typename Scheme>
UrlScheme Url<Scheme>::getScheme() const {
    return m_scheme;
}

template <typename Scheme>
std::string Url<Scheme>::getSchemeName() const {
    return m_schemeName;
}

template <typename Scheme>
bool Url<Scheme>::isValid() const {
    return m_value.find(m_schemePattern) == 0;
}

#endif
