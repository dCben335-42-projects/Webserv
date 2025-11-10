#ifndef URL_HPP
#define URL_HPP

#include <string>
#include <stdexcept>

enum UrlScheme {
    SCHEME_URL_HTTP,
    SCHEME_URL_HTTPS,
    SCHEME_URL_FTP,
    SCHEME_URL_FILE,
    SCHEME_URL_DATA,
    SCHEME_URL_TEL,
    SCHEME_URL_MAILTO,
    SCHEME_URL_WS,
    SCHEME_URL_WSS
};

class Url {
    protected:
        std::string     m_value;
    public:
        explicit Url(const std::string& url);
        virtual ~Url();

        // Accessors
        virtual const std::string& str() const;
        virtual std::string c_str() const;

        virtual std::string schemeStr() const = 0;
        virtual UrlScheme scheme() const = 0;
};

// Scheme structs
struct URL_HTTP   { static const UrlScheme scheme; static const std::string pattern; };
struct URL_HTTPS  { static const UrlScheme scheme; static const std::string pattern; };
struct URL_FTP    { static const UrlScheme scheme; static const std::string pattern; };
struct URL_FILE   { static const UrlScheme scheme; static const std::string pattern; };
struct URL_DATA   { static const UrlScheme scheme; static const std::string pattern; };
struct URL_TEL    { static const UrlScheme scheme; static const std::string pattern; };
struct URL_MAILTO { static const UrlScheme scheme; static const std::string pattern; };
struct URL_WS     { static const UrlScheme scheme; static const std::string pattern; };
struct URL_WSS    { static const UrlScheme scheme; static const std::string pattern; };

// -----------------------------
// Template UrlT
// -----------------------------
template <typename Scheme>
class UrlT : public Url {
    private:
        UrlScheme       m_scheme;
        std::string     m_schemeStr;
    public:
        explicit UrlT(const std::string& url);
        std::string schemeStr() const;
        UrlScheme scheme() const;
        bool isValid(const std::string& url) const;
};

// Specialization declaration for void
template <>
class UrlT<void> : public Url {
    private:
        UrlScheme       m_scheme;
        std::string     m_schemeStr;
    public:
        explicit UrlT(const std::string& url);
        std::string schemeStr() const;
        UrlScheme scheme() const;
        bool isValid(const std::string& url) const;
};

#include "Url.tpp"

#endif // URL_HPP
