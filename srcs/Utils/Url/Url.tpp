#ifndef URL_TPP
#define URL_TPP

#include "Url.hpp"
#include <stdexcept>

// Template constructor
template <typename Scheme>
UrlT<Scheme>::UrlT(const std::string& url) : Url(url) {
    if (!isValid(url)) {
        throw std::invalid_argument("Invalid URL for scheme");
    }
}

template <typename Scheme>
std::string UrlT<Scheme>::schemeStr() const {
    return Scheme::pattern;
}

template <typename Scheme>
UrlScheme UrlT<Scheme>::scheme() const {
    return Scheme::scheme;
}

template <typename Scheme>
bool UrlT<Scheme>::isValid(const std::string& url) const {
    return url.find(Scheme::pattern) == 0;
}

#endif
