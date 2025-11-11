#include "Url.hpp"
#include <sstream>

const UrlScheme URL_HTTP::scheme   = SCHEME_URL_HTTP;
const std::string URL_HTTP::pattern = "http://";
const std::string URL_HTTP::name    = "HTTP";

const UrlScheme URL_HTTPS::scheme  = SCHEME_URL_HTTPS;
const std::string URL_HTTPS::pattern = "https://";
const std::string URL_HTTPS::name    = "HTTPS";

const UrlScheme URL_FTP::scheme    = SCHEME_URL_FTP;
const std::string URL_FTP::pattern = "ftp://";
const std::string URL_FTP::name     = "FTP";

const UrlScheme URL_FILE::scheme = SCHEME_URL_FILE;
const std::string URL_FILE::pattern = "file://";
const std::string URL_FILE::name    = "FILE";

const UrlScheme URL_DATA::scheme = SCHEME_URL_DATA;
const std::string URL_DATA::pattern = "data:";
const std::string URL_DATA::name    = "DATA";

const UrlScheme URL_TEL::scheme = SCHEME_URL_TEL;
const std::string URL_TEL::pattern = "tel:";
const std::string URL_TEL::name    = "TEL";

const UrlScheme URL_MAILTO::scheme = SCHEME_URL_MAILTO;
const std::string URL_MAILTO::pattern = "mailto:";
const std::string URL_MAILTO::name    = "MAILTO";

const UrlScheme URL_WS::scheme     = SCHEME_URL_WS;
const std::string URL_WS::pattern = "ws://";
const std::string URL_WS::name    = "WS";

const UrlScheme URL_WSS::scheme    = SCHEME_URL_WSS;
const std::string URL_WSS::pattern = "wss://";
const std::string URL_WSS::name    = "WSS";

static const SchemeEntry allSchemes[] = {
    { URL_HTTP::scheme,   URL_HTTP::pattern,   URL_HTTP::name },
    { URL_HTTPS::scheme,  URL_HTTPS::pattern,  URL_HTTPS::name },
    { URL_FTP::scheme,    URL_FTP::pattern,    URL_FTP::name },
    { URL_FILE::scheme,   URL_FILE::pattern,   URL_FILE::name },
    { URL_DATA::scheme,   URL_DATA::pattern,   URL_DATA::name },
    { URL_TEL::scheme,    URL_TEL::pattern,    URL_TEL::name },
    { URL_MAILTO::scheme, URL_MAILTO::pattern, URL_MAILTO::name },
    { URL_WS::scheme,     URL_WS::pattern,     URL_WS::name },
    { URL_WSS::scheme,    URL_WSS::pattern,    URL_WSS::name }
};

Url<void>::Url(const std::string& url) : AUrl(url) {
    const size_t schemeCount = sizeof(allSchemes) / sizeof(allSchemes[0]);
    for (size_t i = 0; i < schemeCount; ++i) {
        if (url.find(allSchemes[i].pattern) == 0) {
            m_scheme = allSchemes[i].scheme;
            m_schemePattern = allSchemes[i].pattern;
            m_schemeName = allSchemes[i].name;
            return;
        }
    }
    throw std::invalid_argument("Unknown URL scheme");
}

std::string Url<void>::getSchemePattern() const {
    return m_schemePattern;
}
UrlScheme Url<void>::getScheme() const {
    return m_scheme;
}

std::string Url<void>::getSchemeName() const {
    return m_schemeName;
}

bool Url<void>::isValid() const {
    const size_t schemeCount = sizeof(allSchemes) / sizeof(allSchemes[0]);
    for (size_t i = 0; i < schemeCount; ++i)
        if (m_value.find(allSchemes[i].pattern) == 0)
            return true;
    return false;
}
