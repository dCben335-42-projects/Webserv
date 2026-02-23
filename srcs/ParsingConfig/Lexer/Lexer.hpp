#include "ConfigParser.hpp"
#include "bnf.hpp"

using namespace bnf;

class Lexer
{
	private:
		const SemanticDescriptor m_semanticDescriptor;

	public:
		Lexer(const std::string &content);
		~Lexer();
		HttpConfig parseConfigFile(std::string const &filepath);
};