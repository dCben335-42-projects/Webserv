#ifndef BNF_DEFS_HPP
#define BNF_DEFS_HPP

namespace bnf {
    enum ERuleType {
        SYMBOL,
        GROUP,
        SEQUENCE,
        OPTION
    };

    static const char SEMI   = ';';
    static const char LBRACE = '{';
    static const char RBRACE = '}';
    static const char DQSQ   = '"';
}

#endif
