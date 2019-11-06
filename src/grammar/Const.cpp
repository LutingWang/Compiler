/**********************************************
    > File Name: Const.cpp
    > Author: Luting Wang
    > Mail: 2457348692@qq.com 
    > Created Time: Sat Sep 28 10:45:46 2019
 **********************************************/

#include <cassert>
#include <string>
#include "compiler.h"
#include "error.h"
#include "symtable.h"

#include "basics.h"
#include "Expr.h"

#include "Const.h"
using lexer::getsym;

// <const def> ::= int<iden>=<integer>{,<iden>=<integer>}|char<iden>=<char>{,<iden>=<char>}
void Const::def(void) {
	// do not use `basics::typeId` or it will read a sym
	assert(sym.is(symbol::Type::RESERVED, symbol::INTTK|symbol::CHARTK));
	bool isInt = sym.numIs(symbol::INTTK); 

	// recursively identify identifier and its value
	std::string idenName;
	do {
		getsym();
		assert(sym.is(symbol::Type::IDENFR));
		idenName = sym.str;

		getsym();
		assert(sym.is(symbol::Type::DELIM, symbol::ASSIGN));

		getsym();
		int num = sym.ch;
		if (!isInt && sym.is(symbol::Type::CHARCON)) { getsym(); }
		// error happens if symbol is char or the value is not an integer
		else if (!isInt || !Expr::integer(num)) {
			error::raise(error::Code::EXPECTED_LITERAL);
			// jump to the next ',' or ';'
			while (!sym.is(symbol::Type::DELIM, symbol::COMMA|symbol::SEMICN)) {
				getsym();
			}
		}
		table.pushSym(idenName, true, isInt, num); 
	} while (sym.is(symbol::Type::DELIM, symbol::COMMA));
}

// <const dec> ::= {const<const def>;}
void Const::dec(void) {
	while (sym.is(symbol::Type::RESERVED, symbol::CONSTTK)) {
		getsym();
		def();
		error::assertSymIsSEMICN();
	}
}


