/**********************************************
    > File Name: lexer.h
    > Author: Luting Wang
    > Mail: 2457348692@qq.com 
    > Created Time: Mon Sep 23 10:22:36 2019
 **********************************************/

#ifndef LEXER_H
#define LEXER_H

#include <string>
#include <fstream>

namespace lexer {
	enum Type {
		IDENFR, INTCON, CHARCON, STRCON, RESERVED, DELIM, OPER, COMP
	};

	enum Reserved {
		CONSTTK, INTTK, CHARTK, VOIDTK, MAINTK, IFTK, ELSETK, DOTK, WHILETK, 
		FORTK, SCANFTK, PRINTFTK, RETURNTK
	};

	enum Delim {
		ASSIGN, SEMICN, COMMA, LPARENT, RPARENT, LBRACK, RBRACK, LBRACE, RBRACE
	};

	enum Oper {
		PLUS, MINU, MULT, DIV
	};
	
	enum Comp {
		LSS, LEQ, GRE, GEQ, EQL, NEQ
	};

	struct Symbol {
		// type identifier for the symbol
		Type id;

		// union of symbol value
		char ch;
		unsigned int num;
		std::string str;
	};

	bool getsym();

	void traceback(const Symbol&);
}

extern struct lexer::Symbol sym;

#endif /* LEXER_H */
