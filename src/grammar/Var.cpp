/**********************************************
    > File Name: Var.cpp
    > Author: Luting Wang
    > Mail: 2457348692@qq.com 
    > Created Time: Sat Sep 28 10:53:08 2019
 **********************************************/

#include <cassert>
#include <string>
#include "basics.h"
#include "Var.h"
#include "lexer.h"
#include "symtable.h"
using lexer::getsym;

#include "debug.h" // <>

// <index> ::= '['<unsigned int>']'
// output : identified length of array
unsigned int Var::index(void) {
	assert(sym.id == lexer::DELIM && sym.num == lexer::LBRACK); // ensured by outer function
	getsym();
	assert(sym.id == lexer::INTCON);
	unsigned int result = sym.num;
	getsym();
	assert(sym.id == lexer::DELIM && sym.num == lexer::RBRACK);
	getsym();
	return result;
}

// <var def> ::= <iden>[<index>]{,<iden>[<index>]}
// traceback = true
// input : type of variable is int or char
// output : returned normally or as a result of traceback
bool Var::def(const bool isInt) {
	if (sym.id != lexer::IDENFR) 
		return false;
	lexer::Symbol lastSymbol = sym;
	getsym();
	if (sym.id != lexer::DELIM) {
		lexer::traceback(lastSymbol);
		return false;
	}

	std::string idenName = lastSymbol.str;
	if (sym.num == lexer::LBRACK) {
		table.pushSym(idenName, symtable::VAR, isInt, index());
	} else if (sym.num == lexer::COMMA || sym.num == lexer::SEMICN) {
		table.pushSym(idenName, symtable::VAR, isInt);
	} else {
		lexer::traceback(lastSymbol);
		return false;
	}

	// traceback = false
	while (sym.id == lexer::DELIM && sym.num == lexer::COMMA) {
		getsym();
		assert(sym.id == lexer::IDENFR);
		idenName = sym.str;
		getsym();
		assert(sym.id == lexer::DELIM);
		if (sym.num == lexer::LBRACK) {
			table.pushSym(idenName, symtable::VAR, isInt, index());
		} else {
			table.pushSym(idenName, symtable::VAR, isInt);
		}
	}
	std::string s = v.back(); // <
	v.pop_back();
	v.push_back(print(VAR_DEF));
	v.push_back(s); // >
	return true;
}

// <var dec> ::= {<type id><var def>;}
// traceback = true;
void Var::dec(void) {
	bool isInt;
	bool flag = false; // <>
	for (lexer::Symbol lastSymbol = sym; basics::typeId(isInt); lastSymbol = sym) {
		if (!def(isInt)) {
			lexer::traceback(lastSymbol);
			if (flag) { // <  
				std::string s = v.back(); 
				v.pop_back();
				v.push_back(print(VAR_DEC));
				v.push_back(s); 
			} // >
			return;
		}
		flag = true; // <>
		assert(sym.id == lexer::DELIM && sym.num == lexer::SEMICN);
		getsym();
	}
	if (flag) { // < 
		std::string s = v.back(); 
		v.pop_back();
		v.push_back(print(VAR_DEC));
		v.push_back(s); 
	} // >
}


