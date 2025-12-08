// #include "bnf.hpp"
#include <iostream>
#include "Url.hpp"
#include "config.hpp"
#include "HttpServer.hpp"
#include "AppErrors.hpp"
#include "Method.hpp"
#include "Lexer.hpp"
#include <iostream>
#include "Logger.hpp"

int main()
{
    try {
        try
        {
            Method get("GET");
            HttpServer server(DEFAULT_PORT);
            server.run();   
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