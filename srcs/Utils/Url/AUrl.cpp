#include "AUrl.hpp"
#include <stdexcept>
#include <cstdlib>
// stoi is included via <cstdlib> 
#include <sstream>


AUrl::AUrl(const std::string& url) : m_value(url) {
    parse(url);
    std::stringstream ss(m_port);
    ss >> m_portNumber;
}
AUrl::~AUrl() {}


void AUrl::parse(const std::string& url)
{
    std::string::size_type pos = 0;
    std::string::size_type tmp;

    tmp = url.find("://");
    if (tmp == std::string::npos)
        throw std::invalid_argument("Invalid URL: missing protocol");

    m_protocol = url.substr(0, tmp);
    pos = tmp + 3;

    // --- Find host and optional port
    tmp = url.find_first_of(":/?#", pos);
    if (tmp == std::string::npos) {
        m_host = url.substr(pos);
        return;
    }

    m_host = url.substr(pos, tmp - pos);
    pos = tmp;

    // --- Optional port
    if (url[pos] == ':') {
        pos++;
        tmp = url.find_first_of("/?#", pos);
        if (tmp == std::string::npos) {
            m_port = url.substr(pos);
            return;
        }
        m_port = url.substr(pos, tmp - pos);
        pos = tmp;
    }

    // --- Path
    if (url[pos] == '/') {
        tmp = url.find_first_of("?#", pos);
        if (tmp == std::string::npos) {
            m_path = url.substr(pos);
            return;
        }
        m_path = url.substr(pos, tmp - pos);
        pos = tmp;
    } else {
        m_path = "/";
    }

    // --- Query
    if (url[pos] == '?') {
        pos++;
        tmp = url.find('#', pos);
        if (tmp == std::string::npos) {
            parseQuery(url.substr(pos));
            return;
        }
        parseQuery(url.substr(pos, tmp - pos));
        pos = tmp;
    }

    // --- Fragment
    if (url[pos] == '#') {
        pos++;
        m_fragment = url.substr(pos);
    }
}



void AUrl::parseQuery(const std::string& queryPart)
{
    m_query.clear();
    if (queryPart.empty()) return;

    std::string::size_type start = 0, eq, amp;
    while (start < queryPart.size()) {
        amp = queryPart.find('&', start);
        if (amp == std::string::npos)
            amp = queryPart.size();

        std::string pair = queryPart.substr(start, amp - start);
        eq = pair.find('=');
        if (eq != std::string::npos)
            m_query[pair.substr(0, eq)] = pair.substr(eq + 1);
        else
            m_query[pair] = "";

        start = amp + 1;
    }
}



// ANCHOR getters implementation
const std::string& AUrl::str() const {
    return m_value;
}
std::string AUrl::c_str() const {
    return m_value;
}
const std::string& AUrl::getProtocol() const {
    return m_protocol;
}
const std::string& AUrl::getHost() const {
    return m_host;
}
const std::string& AUrl::getPort() const {
    return m_port;
}
const unsigned short& AUrl::getPortNumber() const {
    return m_portNumber;
}
const std::string& AUrl::getPath() const {
    return m_path;
}
const std::map<std::string, std::string>& AUrl::getQuery() const {
    return m_query;
}
const std::string& AUrl::getFragment() const {
    return m_fragment;
}

std::string AUrl::getQueryParam(const std::string& key) const {
    std::map<std::string, std::string>::const_iterator it = m_query.find(key);
    if (it != m_query.end()) {
        return it->second;
    }
    throw std::out_of_range("Query parameter not found");
}

bool AUrl::hasQueryParam(const std::string& key) const {
    return m_query.find(key) != m_query.end();
}