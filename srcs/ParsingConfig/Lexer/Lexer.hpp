#include <string>
#include <memory>
#include <iostream>
#include <map>
#include <vector>

class Lexer {
    public:
        Lexer(std::string const& filepath);
        ~Lexer();

        // Return the raw content of the file provided to the constructor
        const std::string& getContent() const;

    private:
        // stores the whole file content read by the constructor
        std::string m_content;
};