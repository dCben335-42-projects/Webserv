#ifndef HEADERS_HPP
#define HEADERS_HPP

#include <map>
#include <string>

class Headers {
    private:
        std::map<std::string, std::string> m_headers;
    public:
        Headers();
        ~Headers();

        void addHeader(const std::string& key, const std::string& value);
        std::string getHeader(const std::string& key) const;
        bool hasHeader(const std::string& key) const;
        void removeHeader(const std::string& key);
        std::map<std::string, std::string> getAllHeaders() const;
};