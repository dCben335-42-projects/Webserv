// #include "bnf.hpp"
#include <iostream>
#include "Url.hpp"
#include "config.hpp"
#include "HttpServer.hpp"
#include "AppErrors.hpp"
#include "Method.hpp"


void printError(const std::string& message)
{
    std::cerr << RED << message << RESET << std::endl;
}

void printSuccess(const std::string& message)
{
    std::cout << GREEN << message << RESET << std::endl;
}

void printUrl(const AUrl& url)
{
    std::cout << BLUE << "URL: " << url.str() << RESET << std::endl;
    std::cout << "  Scheme:   " << url.getProtocol() << std::endl;
    std::cout << "  Host:     " << url.getHost() << std::endl;
    std::cout << "  Port:     " << url.getPort() << std::endl;
    std::cout << "  Path:     " << url.getPath() << std::endl;
    std::cout << "  Fragment: " << url.getFragment() << std::endl;
    std::cout << "  Query Parameters:" << std::endl;
    const std::map<std::string, std::string>& query = url.getQuery();
    for (std::map<std::string, std::string>::const_iterator it = query.begin(); it != query.end(); ++it)
    {
        std::cout << "    " << it->first << " = " << it->second << std::endl;
    }
}
#include "Lexer.hpp"
#include <iostream>

int main()
{
    try {
        try
        {
            Method get("GET");
            HttpServer server(9058);
            server.run();   
            // {
            //     Url<void> url("https://www.example.com:8080/path/to/resource?key1=value1&key2=value2#section1");
            //     printUrl(url);
            // }
            // {
            //     Url<URL_HTTP> url("http://example.com");
            //     printUrl(url);
            // }
            // {
            //     Url<URL_FTP> url("ftp://invalid-url.com");
            //     printUrl(url);
            // }
            // {
            //     Url<void> url("wssa://another-example.org/resource?id=123&lang=en2#fragment?test=yes");
            //     printUrl(url);
            // }
        }
        catch(const std::exception& e)
        {
            throw AppError("URL_ERROR", e.what(), 1001, __FILE__, __LINE__);
        }
    }
    catch(const AppError& e)
    {
        std::cerr << e.what() << std::endl;
    }
}