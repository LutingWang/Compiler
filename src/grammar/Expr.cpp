/**********************************************
    > File Name: Expr.cpp
    > Author: Luting Wang
    > Mail: 2457348692@qq.com 
    > Created Time: Sat Sep 28 10:48:30 2019
 **********************************************/

#include <cassert>
#include "compiler.h"
#include "basics.h"
#include "Expr.h"
#include "Func.h"
#include "symtable.h"
#include "error.h"
using lexer::getsym;

// <integer> ::= [<add op>]<unsigned int>
// input : inout parameter for the identified integer
// output : identified an integer
bool Expr::integer(int& result) {
	bool neg;
	basics::add(neg);
	if (!sym.is(symbol::INTCON)) { return false; }
	result = neg ? -((int) sym.num) : sym.num;
	getsym();
	return true;
}

// <factor> ::= <iden>['['<expr>']']|'('<expr>')'|<integer>|<char>|<func call>
bool Expr::factor(void) {
	bool isInt = true;
	switch (sym.id) {
	case symbol::DELIM: 
		assert(sym.numIs(symbol::LPARENT));
		getsym();
		expr();
		error::assertSymIsRPARENT();
		break;
	case symbol::OPER: case symbol::INTCON:
		int num;
		assert(integer(num));
		break;
	case symbol::CHARCON:
		isInt = false;
		getsym();
		break;
	case symbol::IDENFR: {
			std::string name = sym.str;
			getsym();
			if (sym.is(symbol::DELIM, symbol::LPARENT)) {
				const symtable::FuncTable* ft = table.findFunc(name);
				if (ft != nullptr) {
					assert(!ft->isVoid);
					isInt = ft->isInt;
				}
				Func::argValues(ft);
			} else {
				const symtable::Entry* entry = table.findSym(name);
				if (entry == nullptr) { err << error::NODEF << std::endl; }
				else { isInt = entry->isInt; }
				if (sym.is(symbol::DELIM, symbol::LBRACK)) {
					getsym();
					if (!expr()) { err << error::ILLEGAL_IND << std::endl; }
					error::assertSymIsRBRACK();
				} else {
					// TODO: check and use the symbol
				}
			}
		}
		break;
	default: assert(0);
	}
	return isInt;
}

// <item> ::= <factor>{<mult op><factor>}
bool Expr::item(void) {
	bool isMult;
	bool isInt = factor();
	if (basics::mult(isMult)) { 
		isInt = true; 
		do { factor(); } while (basics::mult(isMult));
	} 
	return isInt;
}

// <expr> ::= [<add op>]<item>{<add op><item>}
bool Expr::expr(void) {
	bool neg;
	bool isInt = basics::add(neg);
	isInt = item() || isInt;
	if (basics::add(neg)) {
		isInt = true;
		do { item(); } while (basics::add(neg));
	}
	return isInt;
}
