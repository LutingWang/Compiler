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
#include "MidCode.h"
#include "symtable.h"
#include "error.h"
using lexer::getsym;

// <integer> ::= [<add op>]<unsigned int>
// input : inout parameter for the identified integer
// output : identified an integer
bool Expr::integer(int& result) {
	bool neg;
	basics::add(neg);
	if (!sym.is(symbol::Type::INTCON)) { return false; }
	result = neg ? -((int) sym.num) : sym.num;
	getsym();
	return true;
}

// <factor> ::= <iden>['['<expr>']']|'('<expr>')'|<integer>|<char>|<func call>
symtable::Entry* Expr::factor(void) {
	symtable::Entry* t0;
	switch (sym.id) {
	case symbol::Type::DELIM: 
		assert(sym.numIs(symbol::LPARENT));
		getsym();
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
		t0 = MidCode::genConst(false, sym.ch);
		getsym();
		break;
	case symbol::Type::IDENFR: {
			std::string name = sym.str;
			getsym();
			if (sym.is(symbol::Type::DELIM, symbol::LPARENT)) {
				t0 = Func::argValues(table.findFunc(name));
			} else if (sym.is(symbol::Type::DELIM, symbol::LBRACK)) {
				symtable::Entry* t1 = table.findSym(name);
				if (t1 == nullptr) { err << error::Code::NODEF << std::endl; }
				else { assert(t1->isArray()); }
				getsym();
				symtable::Entry* t2 = expr();
				if (!t2->isInt) { err << error::Code::ILLEGAL_IND << std::endl; }
				error::assertSymIsRBRACK();
				t0 = MidCode::genVar(t1 == nullptr || t1->isInt);
				MidCode::gen(MidCode::Instr::LOAD_IND, t0, t1, t2); // t0 = t1[t2];
			} else {
				t0 = table.findSym(name);
				if (t0 == nullptr) { 
					err << error::Code::NODEF << std::endl; 
					t0 = MidCode::genConst(true, 0);
				} else { assert(!t0->isArray()); }
			}
		}
		break;
	default: assert(0);
	}
	assert(t0 != nullptr);
	return t0;
}

// <item> ::= <factor>{<mult op><factor>}
symtable::Entry* Expr::item(void) {
	symtable::Entry* t0 = nullptr;
	symtable::Entry* t2 = factor();
	bool isMult;
	if (basics::mult(isMult)) { 
		t0 = MidCode::genVar(true);
		do { 
			t2 = factor(); 
			MidCode::gen(isMult ? MidCode::Instr::MULT : 
					MidCode::Instr::DIV, t0, t0, t2); // t0 = t0 [*/] t2
		} while (basics::mult(isMult));
	} 
	assert(t2 != nullptr);
	return t0 == nullptr ? t2 : t0;
}

// <expr> ::= [<add op>]<item>{<add op><item>}
symtable::Entry* Expr::expr(void) {
	symtable::Entry* t0 = nullptr;
	bool neg;
	if (basics::add(neg)) {
		t0 = MidCode::genVar(true);
		if (neg) { MidCode::gen(MidCode::Instr::SUB, t0, nullptr, item()); } // t0 = -t2
	} else {
		symtable::Entry* t1 = item();
		assert(t1 != nullptr);
		if (basics::add(neg)) {
			t0 = MidCode::genVar(true);
			MidCode::gen(neg ? MidCode::Instr::SUB : MidCode::Instr::ADD, t0, t1, item()); // t0 = t1 [+-] t2
		} else { return t1; }
	}

	assert(t0 != nullptr);
	while (basics::add(neg)) { 
		MidCode::gen(neg ? MidCode::Instr::SUB : MidCode::Instr::ADD, t0, t0, item()); // t0 = t0 [+-] t2
	}
	return t0;
}
