/**********************************************
    > File Name: Const.cpp
    > Author: Luting Wang
    > Mail: 2457348692@qq.com 
    > Created Time: Sat Sep 28 10:45:46 2019
 **********************************************/

#include <cassert>
#include <string>
#include "Const.h"
#include "Expr.h"
#include "lexer.h"
#include "symtable.h"
using lexer::getsym;

#include "debug.h" // <>

// <const def> ::= int<iden>=<integer>{,<iden>=<integer>}|char<iden>=<char>{,<iden>=<char>}
void Const::def(void) {
	// identify type (int or char)
	assert(sym.id == lexer::RESERVED);
	bool isInt = false;
	switch (sym.num) {
	case lexer::INTTK: isInt = true;
	case lexer::CHARTK: break;
	default: assert(0);
	}

	// recursively identify identifier and its value
	std::string idenName;
	do {
		getsym();
		assert(sym.id == lexer::IDENFR);
		idenName = sym.str;
		getsym();
		assert(sym.id == lexer::DELIM && sym.num == lexer::ASSIGN);
		getsym();
		if (isInt) {
			table.pushSym(idenName, symtable::CONST, isInt, Expr::integer());
		} else {
			assert(sym.id == lexer::CHARCON);
			table.pushSym(idenName, symtable::CONST, isInt, sym.ch);
			getsym();
		}
	} while (sym.id == lexer::DELIM && sym.num == lexer::COMMA);
	std::string s = v.back(); // <
	v.pop_back();
	v.push_back(print(CONST_DEF));
	v.push_back(s); // >
}

// <const dec> ::= {const<const def>;}
void Const::dec(void) {
	if (sym.id != lexer::RESERVED || sym.num != lexer::CONSTTK) { // <
		return;
	} // >
	while (sym.id == lexer::RESERVED && sym.num == lexer::CONSTTK) {
		getsym();
		def();
		assert(sym.id == lexer::DELIM && sym.num == lexer::SEMICN);
		getsym();
	}
	std::string s = v.back(); // <
	v.pop_back();
	v.push_back(print(CONST_DEC));
	v.push_back(s); // >
}


