// #include "bnf.hpp"
#include <iostream>
#include "Url.hpp"
#include "config.hpp"
#include "HttpServer.hpp"
#include "AppErrors.hpp"

#include "Lexer.hpp"
#include <iostream>
#include "Logger.hpp"

int main()
{
    // bnf::ERuleType type = bnf::GROUP;
    // bnf::ARule *rule = new bnf::Group();

    // std::cout << "Rule type: " << rule->getType() << std::endl;
    // std::cout << "Rule type: " << type<< std::endl;
    // delete rule;
    Lexer("config/def_language.txt");
    std::cout << std::endl;
    // try {
    //     try
    //     {
    //         HttpServer server(9058);
    //         server.run();   
    //         {
    //             Url<void> url("https://www.example.com:8080/path/to/resource?key1=value1&key2=value2#section1");
    //             printUrl(url);
    //         }
    //         {
    //             Url<URL_HTTP> url("http://example.com");
    //             printUrl(url);
    //         }
    //         {
    //             Url<URL_FTP> url("ftp://invalid-url.com");
    //             printUrl(url);
    //         }
    //         {
    //             Url<void> url("wssa://another-example.org/resource?id=123&lang=en2#fragment?test=yes");
    //             printUrl(url);
    //         }
    //     }
    //     catch(const std::exception& e)
    //     {
    //         throw AppError("URL_ERROR", e.what(), 1001, __FILE__, __LINE__);
    //     }
    // }
    // catch(const AppError& e)
    // {
    //     std::cerr << e.what() << std::endl;
    // }
    return (0);
}