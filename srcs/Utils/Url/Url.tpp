#ifndef URL_TPP
#define URL_TPP

#include "Url.hpp"
#include <stdexcept>

template <typename Scheme>
UrlT<Scheme>::UrlT(const std::string& url) : 
    Url(url),
    m_scheme(Scheme::scheme),
    m_schemeStr(Scheme::pattern)
{
    if (!isValid(url))
        throw std::invalid_argument("Invalid URL for scheme");
}

template <typename Scheme>
std::string UrlT<Scheme>::schemeStr() const {
    return m_schemeStr;
}

template <typename Scheme>
UrlScheme UrlT<Scheme>::scheme() const {
    return m_scheme;
}

template <typename Scheme>
bool UrlT<Scheme>::isValid(const std::string& url) const {
    return url.find(Scheme::pattern) == 0;
}

#endif
