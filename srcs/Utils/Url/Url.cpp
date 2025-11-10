#include "Url.hpp"

// -----------------------------
// Base Url implementation
// -----------------------------
Url::Url(const std::string& url) : m_value(url) {}
Url::~Url() {}

const std::string& Url::str() const { return m_value; }
std::string Url::c_str() const { return m_value; }


// -----------------------------
const UrlScheme URL_HTTP::scheme   = SCHEME_URL_HTTP;
const std::string URL_HTTP::pattern = "http://";

const UrlScheme URL_HTTPS::scheme  = SCHEME_URL_HTTPS;
const std::string URL_HTTPS::pattern = "https://";

const UrlScheme URL_FTP::scheme    = SCHEME_URL_FTP;
const std::string URL_FTP::pattern = "ftp://";

const UrlScheme URL_FILE::scheme = SCHEME_URL_FILE;
const std::string URL_FILE::pattern = "file://";

const UrlScheme URL_DATA::scheme = SCHEME_URL_DATA;
const std::string URL_DATA::pattern = "data:";

const UrlScheme URL_TEL::scheme = SCHEME_URL_TEL;
const std::string URL_TEL::pattern = "tel:";

const UrlScheme URL_MAILTO::scheme = SCHEME_URL_MAILTO;
const std::string URL_MAILTO::pattern = "mailto:";

const UrlScheme URL_WS::scheme     = SCHEME_URL_WS;
const std::string URL_WS::pattern = "ws://";

const UrlScheme URL_WSS::scheme    = SCHEME_URL_WSS;
const std::string URL_WSS::pattern = "wss://";

// UrlT<void> specialization implementation
struct SchemeEntry {
    UrlScheme scheme;
    const char* pattern;
};

static const SchemeEntry allSchemes[] = {
    { URL_HTTP::scheme,   URL_HTTP::pattern.c_str() },
    { URL_HTTPS::scheme,  URL_HTTPS::pattern.c_str() },
    { URL_FTP::scheme,    URL_FTP::pattern.c_str() },
    { URL_FILE::scheme,   URL_FILE::pattern.c_str() },
    { URL_DATA::scheme,   URL_DATA::pattern.c_str() },
    { URL_TEL::scheme,    URL_TEL::pattern.c_str() },
    { URL_MAILTO::scheme, URL_MAILTO::pattern.c_str() },
    { URL_WS::scheme,     URL_WS::pattern.c_str() },
    { URL_WSS::scheme,    URL_WSS::pattern.c_str() }
};

UrlT<void>::UrlT(const std::string& url) : Url(url), m_scheme(URL_HTTP::scheme), m_schemeStr("") {
    bool found = false;
    for (size_t i = 0; i < sizeof(allSchemes)/sizeof(allSchemes[0]); ++i) {
        if (url.find(allSchemes[i].pattern) == 0) {
            m_scheme = allSchemes[i].scheme;
            m_schemeStr = allSchemes[i].pattern;
            found = true;
            break;
        }
    }
    if (!found)
        throw std::invalid_argument("Unknown URL scheme");
}

std::string UrlT<void>::schemeStr() const { return m_schemeStr; }
UrlScheme UrlT<void>::scheme() const { return m_scheme; }

bool UrlT<void>::isValid(const std::string& url) const {
    for (size_t i = 0; i < sizeof(allSchemes)/sizeof(allSchemes[0]); ++i)
        if (url.find(allSchemes[i].pattern) == 0)
            return true;
    return false;
}
