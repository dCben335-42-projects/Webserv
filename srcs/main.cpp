#include <iostream>
#include "Url.hpp"
#include "config.hpp"
#include "HttpServer.hpp"

void printError(const std::string& message)
{
    std::cerr << RED << message << RESET << std::endl;
}

void printSuccess(const std::string& message)
{
    std::cout << GREEN << message << RESET << std::endl;
}

void printUrl(const Url& url)
{
    std::cout << BLUE << "URL: " << url.str() << RESET << std::endl;
}

int main()
{
    HttpServer server;
    try
    {
        {
            UrlT<URL_HTTP> url("http://example.com");
            printUrl(url);
        }
        {
            UrlT<URL_FTP> url("ftp://invalid-url.com");
            printUrl(url);
        }
        {
            UrlT<URL_HTTP> url("http://another-example.org/resource");
            printUrl(url);
        }
    }
    catch(const std::exception& e)
    {
        printError(e.what());
    }
}