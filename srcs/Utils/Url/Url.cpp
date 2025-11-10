#include "Url.hpp"

// Base Url implementation
Url::Url(const std::string& url) : m_value(url) {}
Url::~Url() {}

const std::string& Url::str() const { 
    return m_value;
}

std::string Url::c_str() const {
    return m_value;
}

// Define static members for schemes
const UrlScheme URL_HTTP::scheme   = SCHEME_URL_HTTP;
const std::string URL_HTTP::pattern = "http://";

const UrlScheme URL_HTTPS::scheme  = SCHEME_URL_HTTPS;
const std::string URL_HTTPS::pattern = "https://";

const UrlScheme URL_FTP::scheme    = SCHEME_URL_FTP;
const std::string URL_FTP::pattern = "ftp://";

const UrlScheme URL_FILE::scheme = SCHEME_URL_FILE;
const std::string URL_FILE::pattern = "file://";

const UrlScheme URL_MAILTO::scheme = SCHEME_URL_MAILTO;
const std::string URL_MAILTO::pattern = "mailto:";

const UrlScheme URL_WS::scheme     = SCHEME_URL_WS;
const std::string URL_WS::pattern = "ws://";

const UrlScheme URL_WSS::scheme    = SCHEME_URL_WSS;
const std::string URL_WSS::pattern = "wss://";
