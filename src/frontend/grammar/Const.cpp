/**********************************************
    > File Name: Const.cpp
    > Author: Luting Wang
    > Mail: 2457348692@qq.com 
    > Created Time: Sat Sep 28 10:45:46 2019
 **********************************************/

#include <cassert>
#include <string>
#include "symtable/table.h"
#include "symtable/SymTable.h"

#include "../include/errors.h"
#include "../include/Lexer.h"

#include "basics.h"
#include "Expr.h"

#include "Const.h"

// <const def> ::= int<iden>=<integer>{,<iden>=<integer>}|char<iden>=<char>{,<iden>=<char>}
void Const::def(void) {
	// do not use `basics::typeId` or it will read a sym
	assert(sym.is(symbol::Type::RESERVED, symbol::INTTK|symbol::CHARTK));
	const bool isInt = sym.numIs(symbol::INTTK); 

	// recursively identify identifier and its value
	do {
		Lexer::getsym();
		assert(sym.is(symbol::Type::IDENFR));
		const std::string idenName = sym.str();

		Lexer::getsym();
		assert(sym.is(symbol::Type::DELIM, symbol::ASSIGN));

		Lexer::getsym();
		int num = sym.ch();
		if (!isInt && sym.is(symbol::Type::CHARCON)) { Lexer::getsym(); }
		// error happens if symbol is char or the value is not an integer
		else if (!isInt || !Expr::integer(num)) {
			error::raise(error::Code::EXPECTED_LITERAL);
			// jump to the next ',' or ';'
			while (!sym.is(symbol::Type::DELIM, symbol::COMMA|symbol::SEMICN)) {
				Lexer::getsym();
			}
		}
        SymTable::getTable().curTable().pushConst(idenName, isInt, num);
	} while (sym.is(symbol::Type::DELIM, symbol::COMMA));
}

// <const dec> ::= {const<const def>;}
void Const::dec(void) {
	while (sym.is(symbol::Type::RESERVED, symbol::CONSTTK)) {
		Lexer::getsym();
		def();
		error::assertSymIsSEMICN();
	}
}
