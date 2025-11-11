#ifndef AURL_HPP
#define AURL_HPP

#include <string>
#include <map>

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

class AUrl {
    protected:
        std::string m_value;
        std::string m_protocol;
        std::string m_host;
        std::string m_port;
        std::string m_path;
        std::map<std::string, std::string> m_query;
        std::string m_fragment;

        void parse(const std::string& url);
        void parseQuery(const std::string& queryPart);
    public:
        explicit AUrl(const std::string& url);
        virtual ~AUrl();

        const std::string& str() const;
        std::string c_str() const;

        const std::string& getProtocol() const;
        const std::string& getHost() const;
        const std::string& getPort() const;
        const std::string& getPath() const;
        const std::map<std::string, std::string>& getQuery() const;
        const std::string& getFragment() const;

        std::string getQueryParam(const std::string& key) const;
        bool hasQueryParam(const std::string& key) const;

        virtual std::string getSchemePattern() const = 0;
        virtual std::string getSchemeName() const = 0;
        virtual UrlScheme getScheme() const = 0;
};

#endif 
