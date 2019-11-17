/**********************************************
    > File Name: Expr.cpp
    > Author: Luting Wang
    > Mail: 2457348692@qq.com 
    > Created Time: Sat Sep 28 10:48:30 2019
 **********************************************/

#include <cassert>
#include "midcode/MidCode.h"
#include "symtable/Entry.h"
#include "symtable/SymTable.h"

#include "../include/errors.h"
#include "../include/Lexer.h"

#include "basics.h"
#include "Func.h"

#include "Expr.h"

// <integer> ::= [<add op>]<unsigned int>
// input : inout parameter for the identified integer
// output : identified an integer
bool Expr::integer(int& result) {
	const symbol::Symbol lastSymbol = sym;
	bool neg;
	if (basics::add(neg) && !sym.is(symbol::Type::INTCON)) {
		Lexer::traceback(lastSymbol);
		return false;
	}
	if (!sym.is(symbol::Type::INTCON)) { return false; }
	result = neg ? -((int) sym.num()) : sym.num();
	Lexer::getsym();
	return true;
}

// <factor> ::= <iden>['['<expr>']']|'('<expr>')'|<integer>|<char>|<func call>
const symtable::Entry* Expr::factor(void) {
	const symtable::Entry* t0 = nullptr;
	switch (sym.id()) {
	case symbol::Type::DELIM: 
		assert(sym.numIs(symbol::LPARENT));
		Lexer::getsym();
		t0 = MidCode::genVar(true);
		MidCode::gen(MidCode::Instr::ASSIGN, t0, expr(), nullptr);
		error::assertSymIsRPARENT();
		break;
	case symbol::Type::OPER: case symbol::Type::INTCON:
		int num;
		assert(integer(num));
		t0 = MidCode::genConst(true, num);
		break;
	case symbol::Type::CHARCON:
		t0 = MidCode::genConst(false, sym.ch());
		Lexer::getsym();
		break;
	case symbol::Type::IDENFR: {
			std::string name = sym.str();
			Lexer::getsym();
			if (sym.is(symbol::Type::DELIM, symbol::LPARENT)) {
                t0 = Func::argValues(SymTable::getTable().findFunc(name));
			} else if (sym.is(symbol::Type::DELIM, symbol::LBRACK)) {
				const symtable::Entry* const t1 = SymTable::getTable().findSym(name);
                assert(t1->isInvalid() || t1->isArray());
				Lexer::getsym();
				const symtable::Entry* const t2 = expr();
				if (!t2->isInt()) { error::raise(error::Code::ILLEGAL_IND); }
				error::assertSymIsRBRACK();
				t0 = MidCode::genVar(t1->isInt());
				MidCode::gen(MidCode::Instr::LOAD_IND, t0, t1, t2); // t0 = t1[t2];
			} else {
				t0 = SymTable::getTable().findSym(name);
                assert(t0->isInvalid() || !t0->isArray());
			}
		}
		break;
	default: assert(0);
	}
	assert(t0 != nullptr);
	return t0;
}

// <item> ::= <factor>{<mult op><factor>}
const symtable::Entry* Expr::item(void) {
	const symtable::Entry* const t1 = factor();
	bool isMult;
	if (!basics::mult(isMult)) { 
		assert(t1 != nullptr);
		return t1;
	} 
	const symtable::Entry* const t0 = MidCode::genVar(true);
	MidCode::gen(isMult ? MidCode::Instr::MULT : MidCode::Instr::DIV, 
			t0, t1, factor()); // t0 = t1 [*/] t2
	while (basics::mult(isMult)) {
		MidCode::gen(isMult ? MidCode::Instr::MULT : MidCode::Instr::DIV, 
				t0, t0, factor()); // t0 = t0 [*/] t2
	}
	return t0;
}

// <expr> ::= [<add op>]<item>{<add op><item>}
const symtable::Entry* Expr::expr(void) {
	const symtable::Entry* t0 = nullptr;
	const symtable::Entry* t1;
	bool neg;
	if (basics::add(neg)) {
		t0 = MidCode::genVar(true);
		t1 = MidCode::genConst(true, 0);
		MidCode::gen(neg ? MidCode::Instr::SUB : MidCode::Instr::ADD, 
				t0, t1, item()); // t0 = 0 [+-] t2
	} else {
		t1 = item();
		if (basics::add(neg)) {
			t0 = MidCode::genVar(true);
			MidCode::gen(neg ? MidCode::Instr::SUB : MidCode::Instr::ADD, 
					t0, t1, item()); // t0 = t1 [+-] t2
		} else {
            return t1;
        }
	}

	while (basics::add(neg)) { 
		MidCode::gen(neg ? MidCode::Instr::SUB : MidCode::Instr::ADD, 
				t0, t0, item()); // t0 = t0 [+-] t2
	}
	return t0;
}
