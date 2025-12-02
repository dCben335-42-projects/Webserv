#include "utils.hpp"

// -----------------------------
// startsWith
// -----------------------------
bool startsWith(const std::string &s, const std::string &prefix)
{
    if(s.size() < prefix.size()) return false;
    return s.compare(0, prefix.size(), prefix) == 0;
}

// -----------------------------
// log() for char* strings
// -----------------------------
void log(const char *s)
{
    if(s)
        std::cout << s;
    else
        std::cout << "null";
}

void log(char *s)
{
    log(static_cast<const char*>(s));
}
