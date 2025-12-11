#ifndef NUMBER_HPP
# define NUMBER_HPP

# include <string>

class Number {
    private:
        Number();
        ~Number();
    public:
        static std::string toString(const int number);
        static int fromString(const std::string &str);
        static bool isNumber(const std::string &str);
        static size_t toSizeT(const std::string &str);
        static int toInt(const std::string &str);
};

#endif