#ifndef SSL_HPP
# define SSL_HPP

#include <string>

enum SslProtocol
{
    SSLv23,
    TLSv1,
    TLSv1_1,
    TLSv1_2,
    TLSv1_3
};

class Ssl
{
    private:
        SslProtocol    m_protocol;
        bool           m_enabled;
        std::string    m_certificatePath;
        std::string    m_privateKeyPath;
        std::string    m_url;
    public:
        Ssl();
        ~Ssl();
};

#endif