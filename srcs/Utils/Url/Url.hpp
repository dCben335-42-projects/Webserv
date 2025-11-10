#ifndef URL_HPP
#define URL_HPP

#include <string>
#include <stdexcept>

enum UrlScheme {
    SCHEME_URL_HTTP,
    SCHEME_URL_HTTPS,
    SCHEME_URL_FTP,
    SCHEME_URL_FILE,
    SCHEME_URL_MAILTO,
    SCHEME_URL_TEL,
    SCHEME_URL_DATA,
    SCHEME_URL_WS,
    SCHEME_URL_WSS
};

class Url {
    protected:
        std::string m_value;
    public:
        Url(const std::string& url);
        virtual ~Url();
        virtual const std::string& str() const;
        virtual std::string c_str() const;

        virtual std::string schemeStr() const = 0;
        virtual UrlScheme scheme() const = 0;
};


struct URL_HTTP   { static const UrlScheme scheme; static const std::string pattern; };
struct URL_HTTPS  { static const UrlScheme scheme; static const std::string pattern; };
struct URL_FTP    { static const UrlScheme scheme; static const std::string pattern; };
struct URL_FILE   { static const UrlScheme scheme; static const std::string pattern; };
struct URL_MAILTO { static const UrlScheme scheme; static const std::string pattern; };
struct URL_WS     { static const UrlScheme scheme; static const std::string pattern; };
struct URL_WSS    { static const UrlScheme scheme; static const std::string pattern; };

template <typename Scheme>
class UrlT : public Url {
    public:
        explicit    UrlT(const std::string& url);
        bool        isValid(const std::string& url) const;

        std::string schemeStr() const;
        UrlScheme   scheme() const;
};

#include "Url.tpp"

#endif
