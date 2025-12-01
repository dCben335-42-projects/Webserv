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
};

#endif