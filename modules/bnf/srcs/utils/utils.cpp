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
        std::cout << Log::indent() << "null";
}

void log(char *s)
{
    log(static_cast<const char*>(s));
}

bool matchClass(char c, const char* cls) {
    for (size_t i = 0; cls[i]; ++i) {
        if (cls[i] == '-' && i > 0 && cls[i+1]) {
            char start = cls[i-1];
            char end   = cls[i+1];
            if (c >= start && c <= end)
                return true;
        }
        else if (c == cls[i]) {
            return true;
        }
    }
    return false;
}

bool miniRegex(const std::string& s, const char* pattern) {
    size_t i = 0; // input
    size_t p = 0; // pattern

    if (pattern[p] == '^') p++; // début de chaîne

    while (pattern[p] && pattern[p] != '$') {
        if (pattern[p] == '[') {
            p++;
            size_t start = p;
            while (pattern[p] && pattern[p] != ']') p++;
            if (pattern[p] != ']') return false;

            bool plus = false, star = false;
            if (pattern[p+1] == '*') star = true;
            else if (pattern[p+1] == '+') plus = true;

            if (plus) {
                if (i >= s.size() || !matchClass(s[i], pattern + start))
                    return false; // au moins 1
                while (i < s.size() && matchClass(s[i], pattern + start))
                    i++;
                p += 2; // skip ]+
            }
            else if (star) {
                while (i < s.size() && matchClass(s[i], pattern + start))
                    i++;
                p += 2; // skip ]*
            }
            else {
                if (i >= s.size() || !matchClass(s[i], pattern + start))
                    return false;
                i++;
                p++;
            }
        }
        else {
            if (i >= s.size() || s[i] != pattern[p]) return false;
            i++;
            p++;
        }
    }

    if (pattern[p] == '$') p++; // fin de chaîne

    return i == s.size() && pattern[p] == '\0';
}


bool is_alpha_string(const std::string& s) {
    if (s.empty()) return false;
    for (size_t i = 0; i < s.size(); ++i)
        if (!std::isalpha(s[i]))
            return false;
    return true;
}

bool validate_string_strict(const std::string& s) {
    size_t len = s.size();
    if (len < 2) return false;

    if (s[0] == '\'' && s[len - 1] == '\'') {
        return true;
    }

    // sinon que lettres
    for (size_t i = 0; i < len; ++i)
        if (!std::isalpha(s[i]) && s[i] != '_')
            return false;

    return true;
}

bool isOnlyWhitespace(const std::string& str, size_t start)
{
    if (start >= str.size())
        return false;
    for (std::string::const_iterator it = str.begin() + start; it != str.end(); ++it) {
        if (!isspace(static_cast<unsigned char>(*it))) {
            return false;
        }
    }
    return true;
}

std::string erasePaternWrapper(std::string &s, const std::string pattern)
{
    return s.erase(0, s.find_first_not_of(pattern)).erase(s.find_last_not_of(pattern) + 1);
}