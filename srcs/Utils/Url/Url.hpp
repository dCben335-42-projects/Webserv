#ifndef URL_HPP
#define URL_HPP

#include "AUrl.hpp"
#include <string>

struct SchemeEntry {
    UrlScheme scheme;
    const std::string pattern;
    const std::string name;
};

// Scheme structs
struct URL_HTTP   { static const UrlScheme scheme; static const std::string pattern; static const std::string name; };
struct URL_HTTPS  { static const UrlScheme scheme; static const std::string pattern; static const std::string name; };
struct URL_FTP    { static const UrlScheme scheme; static const std::string pattern; static const std::string name; };
struct URL_FILE   { static const UrlScheme scheme; static const std::string pattern; static const std::string name; };
struct URL_DATA   { static const UrlScheme scheme; static const std::string pattern; static const std::string name; };
struct URL_TEL    { static const UrlScheme scheme; static const std::string pattern; static const std::string name; };
struct URL_MAILTO { static const UrlScheme scheme; static const std::string pattern; static const std::string name; };
struct URL_WS     { static const UrlScheme scheme; static const std::string pattern; static const std::string name; };
struct URL_WSS    { static const UrlScheme scheme; static const std::string pattern; static const std::string name; };

template <typename Scheme>
class Url : public AUrl {
    private:
        UrlScheme       m_scheme;
        std::string     m_schemePattern;
        std::string     m_schemeName;

    public:
        explicit Url(const std::string& url);

        std::string     getSchemeName() const;
        std::string     getSchemePattern() const;
        UrlScheme       getScheme() const;

        bool isValid(void) const;
};

template <>
class Url<void> : public AUrl {
    private:
        UrlScheme       m_scheme;
        std::string     m_schemePattern;
        std::string     m_schemeName;
    public:
        explicit Url(const std::string& url);

        std::string     getSchemeName() const;
        std::string     getSchemePattern() const;
        UrlScheme       getScheme() const;

        bool isValid(void) const;
};

#include "Url.tpp"

#endif
