/**********************************************
    > File Name: Var.cpp
    > Author: Luting Wang
    > Mail: 2457348692@qq.com 
    > Created Time: Sat Sep 28 10:53:08 2019
 **********************************************/

#include <cassert>
#include <string>
#include "symtable/table.h"
#include "symtable/SymTable.h"

#include "../include/errors.h"
#include "../include/lexer.h"

#include "basics.h"

#include "Var.h"
using lexer::getsym;

// <index> ::= '['<unsigned int>']'
// output : identified length of array
unsigned int Var::index(void) {
	assert(sym.is(symbol::Type::DELIM, symbol::LBRACK)); // ensured by outer function
	getsym();
	assert(sym.is(symbol::Type::INTCON));
	const unsigned int result = sym.num;
	getsym();
	error::assertSymIsRBRACK();
	return result;
}

// <var def> ::= <iden>[<index>]{,<iden>[<index>]}
// traceback = true
// input : type of variable is int or char
// output : returned normally or as a result of traceback
bool Var::def(const bool isInt) {
	if (!sym.is(symbol::Type::IDENFR)) { return false; }
	const symbol::Symbol lastSymbol = sym;
	getsym();
	if (!sym.is(symbol::Type::DELIM)) {
		lexer::traceback(lastSymbol);
		return false;
	}

	std::string idenName = lastSymbol.str;
	if (sym.numIs(symbol::LBRACK)) {
        SymTable::getTable().curTable().pushArray(idenName, isInt, index());
	} else if (sym.numIs(symbol::COMMA|symbol::SEMICN)) {
        SymTable::getTable().curTable().pushVar(idenName, isInt);
	} else {
		lexer::traceback(lastSymbol);
		return false;
	}

	// traceback = false
	while (sym.is(symbol::Type::DELIM, symbol::COMMA)) {
		getsym();
		assert(sym.is(symbol::Type::IDENFR));
		idenName = sym.str;
		getsym();
		assert(sym.is(symbol::Type::DELIM));
		if (sym.numIs(symbol::LBRACK)) {
            SymTable::getTable().curTable().pushArray(idenName, isInt, index());
        } else {
            SymTable::getTable().curTable().pushVar(idenName, isInt);
        }
	}
	return true;
}

// <var dec> ::= {<type id><var def>;}
// traceback = true;
void Var::dec(void) {
	bool isInt;
	for (symbol::Symbol lastSymbol = sym; basics::typeId(isInt); lastSymbol = sym) {
		if (!def(isInt)) {
			lexer::traceback(lastSymbol);
			return;
		}
		error::assertSymIsSEMICN();
	}
}

