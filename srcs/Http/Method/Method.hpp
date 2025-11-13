#ifndef METHOD_H
#define METHOD_H

# include <string>

enum EMethodType
{
    GET,
    POST,
    DELETE,
    PUT,
    PATCH,
    NONE
};

struct MethodType
{
    EMethodType type;
    std::string str;
};

class Method {
    private:
        EMethodType     m_type;
        std::string     m_strType;

    public:
        explicit Method(const std::string& method);
        ~Method();

        Method &operator=(const Method &other);

        EMethodType     &getType(void);
        std::string    &getStrType(void);
        bool isValid(void) const;
};

#endif 