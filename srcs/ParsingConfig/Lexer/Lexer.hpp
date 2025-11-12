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


enum TermValueType {
    ITEM,
    REPETITIVE,
    PARENTHESIS
};

class ATermValue {
    protected:
        const TermValueType m_type;
        ATermValue(TermValueType t) : m_type(t) {}
    public:
        virtual ~ATermValue();

        const TermValueType getType() const;
        virtual void print() const = 0;
};


class Item : public ATermValue {
    public:
        Item();
        Item(const std::string& val);
        ~Item();
        void print() const;
    private:
        std::string m_value;
};
class Repetitive : public ATermValue {
    public:
        Repetitive();
        ~Repetitive();
        void addItem(const std::string& val);
        void print() const;
    private:
        std::vector<std::string> items;
};
class Parenthesis : public ATermValue {
    public:
        Parenthesis();
        ~Parenthesis();
        void addItem(const std::string& val);
        void print() const;
    private:
        std::vector<std::string> items;
};





class TermRule {
    public:
        void addValue(ATermValue &v);
    private:
        std::string key;
        std::vector<ATermValue *> values;
};


// class TermRule {
//     public:
//         TermRule();
//         ~TermRule();

//     private:
//         std::string key;
//         std::vector<Val> values;
// };




// class Item {
//     public:
//         Item();
//         ~Item();

//     private:
//         std::string value;
// };
    
    
// class Block {
//         public:
//         Block();
//         ~Block();
        
//     private:
//         std::vector<std::string> items;
// };





