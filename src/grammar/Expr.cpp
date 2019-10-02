/**********************************************
    > File Name: Expr.cpp
    > Author: Luting Wang
    > Mail: 2457348692@qq.com 
    > Created Time: Sat Sep 28 10:48:30 2019
 **********************************************/

#include <cassert>
#include "basics.h"
#include "Expr.h"
#include "Func.h"
#include "lexer.h"
using lexer::getsym;

#include "debug.h" // <>

// <integer> ::= [<add op>]<unsigned int>
// output : identified integer
int Expr::integer(void) {
	bool neg;
	basics::add(neg);
	assert(sym.id == lexer::INTCON);
	int result = neg ? -sym.num : sym.num;
	v.push_back(print(INT)); // <>
	getsym();
	return result;
}

// <factor> ::= <iden>['['<expr>']']|'('<expr>')'|<integer>|<char>|<func call>
void Expr::factor(void) {
	switch (sym.id) {
	case lexer::DELIM: 
		assert(sym.num == lexer::LPARENT);
		getsym();
		expr();
		assert(sym.id == lexer::DELIM && sym.num == lexer::RPARENT);
		getsym();
		break;
	case lexer::OPER: case lexer::INTCON:
		integer();
		break;
	case lexer::CHARCON:
		getsym();
		break;
	case lexer::IDENFR: {
			lexer::Symbol lastSymbol = sym;
			getsym();
			if (sym.id == lexer::DELIM) {
				switch (sym.num) {
				case lexer::LPARENT:
					lexer::traceback(lastSymbol);
					Func::call(); 
					break;
				case lexer::LBRACK:
					getsym();
					expr();
					assert(sym.id == lexer::DELIM && sym.num == lexer::RBRACK);
					getsym();
					break;
				}
			}
		}
		break;
	default: assert(0);
	}
	std::string s = v.back(); // <
	v.pop_back();
	v.push_back(print(FACTOR)); 
	v.push_back(s); // >
}

// <item> ::= <factor>{<mult op><factor>}
void Expr::item(void) {
	bool isMult;
	do { factor(); } while (basics::mult(isMult));
	std::string s = v.back(); // <
	v.pop_back();
	v.push_back(print(ITEM)); 
	v.push_back(s); // >
}

// <expr> ::= [<add op>]<item>{<add op><item>}
void Expr::expr(void) {
	bool neg;
	basics::add(neg);
	do { item(); } while (basics::add(neg));
	std::string s = v.back(); // <
	v.pop_back();
	v.push_back(print(EXPR)); 
	v.push_back(s); // >
}
