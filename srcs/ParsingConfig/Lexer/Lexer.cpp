#include "Lexer.hpp"

#include <fstream>
#include <sstream>
#include <stdexcept>
#include <iostream>
#include "Logger.hpp"

using namespace bnf;

Lexer::Lexer(std::string const &filepath)
{
	std::ifstream ifs(filepath.c_str(), std::ios::in);
	if (!ifs.is_open())
		throw std::runtime_error("Lexer: cannot open file: " + filepath);
	std::ostringstream ss;
	ss << ifs.rdbuf();
	m_content = ss.str();

	SemanticDescriptor val(m_content);


	std::cout << val;
	Parser parser(val);

	Logger::log("========Parsing========\n");
	try
	{
		// parser.parse("AmD88l2");
		// parser.parse("AmD88l");
		// parser.parse("L2");

		// OE sans ! DONC BON
		// parser.parse("Abb11b1");

		// // // OE avec ! DONC BON
		// parser.parse("Abb11b1!");

		// // // ALL sans dernier LETTER, OE avec DIGIT 1 DONC BON
		// parser.parse("Abb111");

		// // // ALL sans dernier DIGIT,DIGIT,LETTER, OE avec DIGIT 1 DONC BON
		// parser.parse("Abb1");

		// // // ALL sans dernier DIGIT,DIGIT,LETTER, OE avec DIGIT 1 DONC BON
		// parser.parse("Abb11");

		// // IDENT sans ALL, OE sans !
		// parser.parse("A1");

		// // IDENT sans ALL, OE avec !
		// parser.parse("A1!");

		// parser.parse("bsa6!");

		// parser.parse("Zz9");


		// parser.parse("&");
		// parser.parse("-Kn5p");
		// parser.parse("9@====--))---Ab9l-7");
		// parser.parse("====--))---Ab9l-");
		// parser.parse("9@====-Ab-5");
		// parser.parse("9!====Abl-2");
		// parser.parse("9====------Abl-");
		// parser.parse("====-Al-");
		// parser.parse("====-A9l");
		parser.parse("llo_");
	}
	catch (const SemanticException &e)
	{
		std::cout << e;
	}

	ifs.close();
}

Lexer::~Lexer()
{
}

const std::string &Lexer::getContent() const
{
	return m_content;
}
