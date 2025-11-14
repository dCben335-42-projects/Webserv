#ifndef BNF_DEFS_HPP
#define BNF_DEFS_HPP

#include <string>

namespace bnf {
    enum ERuleType {
        SYMBOL,
        GROUP,
        SEQUENCE,
        OPTION
    };

    static const std::string SEMI   = ";";
    static const std::string LBRACKET = "{";
    static const std::string RBRACKET = "}";
    static const std::string LBRACE = "(";
    static const std::string RBRACE = ")";
    static const std::string LHOOK = "[";
    static const std::string RHOOK = "]";
    static const std::string DQSQ   = "'";
    static const std::string DIVIDER = "|";
    static const std::string EQRULE   = "::=";
    static const std::string ORRULE   = "|=";
    static const std::string WHITESPACE   = "\t\n\r";
}

#endif
