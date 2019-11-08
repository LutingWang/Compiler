/**********************************************
    > File Name: Expr.cpp
    > Author: Luting Wang
    > Mail: 2457348692@qq.com 
    > Created Time: Sat Sep 28 10:48:30 2019
 **********************************************/

#include <cassert>
#include "compiler.h"
#include "error.h"
#include "midcode.h"
#include "symtable.h"

#include "basics.h"
#include "Func.h"

#include "Expr.h"
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
				if (t1 == nullptr) { error::raise(error::Code::NODEF); }
				else { assert(t1->isArray()); }
				getsym();
				symtable::Entry* t2 = expr();
				if (!t2->isInt) { error::raise(error::Code::ILLEGAL_IND); }
				error::assertSymIsRBRACK();
				t0 = MidCode::genVar(t1 == nullptr || t1->isInt);
				MidCode::gen(MidCode::Instr::LOAD_IND, t0, t1, t2); // t0 = t1[t2];
			} else {
				t0 = table.findSym(name);
				if (t0 == nullptr) { 
					error::raise(error::Code::NODEF); 
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
	symtable::Entry* t1 = factor();
	bool isMult;
	if (!basics::mult(isMult)) { 
		assert(t1 != nullptr);
		return t1;
	} 
	symtable::Entry* t0 = MidCode::genVar(true);
	MidCode::gen(isMult ? MidCode::Instr::MULT : MidCode::Instr::DIV, 
			t0, t1, factor()); // t0 = t1 [*/] t2
	while (basics::mult(isMult)) {
		MidCode::gen(isMult ? MidCode::Instr::MULT : MidCode::Instr::DIV, 
				t0, t0, factor()); // t0 = t0 [*/] t2
	}
	return t0;
}

// <expr> ::= [<add op>]<item>{<add op><item>}
symtable::Entry* Expr::expr(void) {
	symtable::Entry* t0 = nullptr;
	symtable::Entry* t1;
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
		} else { return t1; }
	}

	while (basics::add(neg)) { 
		MidCode::gen(neg ? MidCode::Instr::SUB : MidCode::Instr::ADD, 
				t0, t0, item()); // t0 = t0 [+-] t2
	}
	return t0;
}
