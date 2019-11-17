/**********************************************
    > File Name: Stat.cpp
    > Author: Luting Wang
    > Mail: 2457348692@qq.com 
    > Created Time: Sat Sep 28 10:43:53 2019
 **********************************************/

#include <cassert>
#include <map>
#include "midcode/MidCode.h"
#include "symtable/table.h"
#include "symtable/Entry.h"
#include "symtable/SymTable.h"

#include "../include/errors.h"
#include "../include/Lexer.h"

#include "basics.h"
#include "Const.h"
#include "Expr.h"
#include "Func.h"
#include "Var.h"

#include "Stat.h"

namespace {
	MidCode::Instr translate(symbol::Comp comp, bool takeNot) {
		switch (comp) {
#define CASE(id, pos, neg) \
		case symbol::id: \
			return takeNot ? MidCode::Instr::neg : MidCode::Instr::pos
		CASE(LSS, BLT, BGE);
		CASE(LEQ, BLE, BGT);
		CASE(GRE, BGT, BLE);
		CASE(GEQ, BGE, BLT);
		CASE(EQL, BEQ, BNE);
		CASE(NEQ, BNE, BEQ);
#undef CASE
		default: assert(0);
		}
	}
}

// <cond> ::= <expr>[<comp op><expr>]
void Stat::Cond::cond(const bool branchIfNot, const std::string& labelName) {
	const symtable::Entry* const t1 = Expr::expr();
	if (!t1->isInt()) { error::raise(error::Code::MISMATCHED_COND_TYPE); }
	if (sym.is(symbol::Type::COMP)) {
		MidCode::Instr comp = translate(static_cast<symbol::Comp>(sym.num()), branchIfNot);
		Lexer::getsym();
		const symtable::Entry* const t2 = Expr::expr();
		if (!t2->isInt()) { error::raise(error::Code::MISMATCHED_COND_TYPE); }
		MidCode::gen(comp, nullptr, t1, t2, labelName);
	} else {
		MidCode::gen(branchIfNot ? MidCode::Instr::BEQ : MidCode::Instr::BNE, nullptr, t1, nullptr, labelName);
	}
}

// <if stat> ::= if'('<cond>')'<stat>[else<stat>]
bool Stat::Cond::_if(void) {
	assert(sym.is(symbol::Type::RESERVED, symbol::IFTK)); // ensured by outer function
	Lexer::getsym();
	assert(sym.is(symbol::Type::DELIM, symbol::LPARENT));
	Lexer::getsym();
	std::string labelElse = MidCode::genLabel();
	cond(true, labelElse);
	error::assertSymIsRPARENT();

	bool hasRet = stat();
	if (sym.is(symbol::Type::RESERVED, symbol::ELSETK)) {
		std::string labelEnd = MidCode::genLabel();
		MidCode::gen(MidCode::Instr::GOTO, nullptr, nullptr, nullptr, labelEnd);
		MidCode::gen(MidCode::Instr::LABEL, nullptr, nullptr, nullptr, labelElse);
		Lexer::getsym();
		hasRet = stat() && hasRet; // ensures that stat() is executed
		MidCode::gen(MidCode::Instr::LABEL, nullptr, nullptr, nullptr, labelEnd);
		return hasRet;
	} else {
		MidCode::gen(MidCode::Instr::LABEL, nullptr, nullptr, nullptr, labelElse);
		return false;
	}
}

// <while stat> ::= while'('<cond>')'<stat>
void Stat::Cond::_while(void) {
	assert(sym.is(symbol::Type::RESERVED, symbol::WHILETK)); // ensured by outer function
	Lexer::getsym();
	assert(sym.is(symbol::Type::DELIM, symbol::LPARENT));
	Lexer::getsym();
	std::string labelBegin = MidCode::genLabel();
	MidCode::gen(MidCode::Instr::LABEL, nullptr, nullptr, nullptr, labelBegin);
	std::string labelEnd = MidCode::genLabel();
	cond(true, labelEnd);
	error::assertSymIsRPARENT();
	stat();
	MidCode::gen(MidCode::Instr::GOTO, nullptr, nullptr, nullptr, labelBegin);
	MidCode::gen(MidCode::Instr::LABEL, nullptr, nullptr, nullptr, labelEnd);
}

// <do stat> ::= do<stat>while'('<cond>')'
bool Stat::Cond::_do(void) {
	assert(sym.is(symbol::Type::RESERVED, symbol::DOTK)); // ensured by outer function
	Lexer::getsym();
	std::string labelBegin = MidCode::genLabel();
	MidCode::gen(MidCode::Instr::LABEL, nullptr, nullptr, nullptr, labelBegin);
	bool hasRet = stat();
	if (sym.is(symbol::Type::RESERVED, symbol::WHILETK)) { Lexer::getsym(); }
	else { error::raise(error::Code::MISSING_WHILE); }
	assert(sym.is(symbol::Type::DELIM, symbol::LPARENT));
	Lexer::getsym();
	cond(false, labelBegin);
	error::assertSymIsRPARENT();
	return hasRet;
}

// <for stat> ::= for'('<iden>=<expr>;<cond>;<iden>=<iden><add op><unsigned int>')'<stat>
void Stat::Cond::_for(void) {
	const symtable::Entry* t0;
	const symtable::Entry* t1;

	// for'('
	assert(sym.is(symbol::Type::RESERVED, symbol::FORTK)); // ensured by outer function
	Lexer::getsym();
	assert(sym.is(symbol::Type::DELIM, symbol::LPARENT));
	Lexer::getsym();

	// <iden>=<expr>;
	assert(sym.is(symbol::Type::IDENFR));
	t0 = SymTable::getTable().findSym(sym.str());
	if (t0->isConst()) { error::raise(error::Code::ILLEGAL_ASSIGN); }
	else { assert(t0->isInvalid() || !t0->isArray()); }
	Lexer::getsym();
	assert(sym.is(symbol::Type::DELIM, symbol::ASSIGN));
	Lexer::getsym();
	t1 = Expr::expr();
	MidCode::gen(MidCode::Instr::ASSIGN, t0, t1, nullptr);
	error::assertSymIsSEMICN();

	// <cond>;
	std::string labelBegin = MidCode::genLabel();
	MidCode::gen(MidCode::Instr::LABEL, nullptr, nullptr, nullptr, labelBegin);
	std::string labelEnd = MidCode::genLabel();
	cond(true, labelEnd);
	error::assertSymIsSEMICN();

	// <iden>=<iden><add op><unsigned int>')'
	assert(sym.is(symbol::Type::IDENFR));
	t0 = SymTable::getTable().findSym(sym.str());
	if (t0->isConst()) { error::raise(error::Code::ILLEGAL_ASSIGN); }
	else { assert(t0->isInvalid() || !t0->isArray()); }
	Lexer::getsym();
	assert(sym.is(symbol::Type::DELIM, symbol::ASSIGN));
	Lexer::getsym();
	assert(sym.is(symbol::Type::IDENFR));
	t1 = SymTable::getTable().findSym(sym.str());
	assert(t1->isInvalid() || !t1->isArray());
	Lexer::getsym();
	bool minus;
	assert(basics::add(minus));
	assert(sym.is(symbol::Type::INTCON));
	const symtable::Entry* const stepSize = MidCode::genConst(true, sym.num());
	Lexer::getsym();
	error::assertSymIsRPARENT();

	stat();

	MidCode::gen(minus ? MidCode::Instr::SUB : MidCode::Instr::ADD, t0, t1, stepSize);
	MidCode::gen(MidCode::Instr::GOTO, nullptr, nullptr, nullptr, labelBegin);
	MidCode::gen(MidCode::Instr::LABEL, nullptr, nullptr, nullptr, labelEnd);
}

// <read stat> ::= scanf'('<iden>{,<iden>}')'
void Stat::read(void) {
	assert(sym.is(symbol::Type::RESERVED, symbol::SCANFTK)); // ensured by outer function
	Lexer::getsym();
	assert(sym.is(symbol::Type::DELIM, symbol::LPARENT));
	do {
		Lexer::getsym();
		assert(sym.is(symbol::Type::IDENFR));
		const symtable::Entry* const t0 = SymTable::getTable().findSym(sym.str());
		if (t0->isConst()) { error::raise(error::Code::ILLEGAL_ASSIGN); }
		else { assert(t0->isInvalid() || !t0->isArray()); }
		MidCode::gen(MidCode::Instr::INPUT, t0, nullptr, nullptr);
		Lexer::getsym();
	} while (sym.is(symbol::Type::DELIM, symbol::COMMA));
	error::assertSymIsRPARENT();
}

// <write stat> ::= printf'('<string>[,<expr>]')'|printf'('<expr>')'
void Stat::write(void) {
	assert(sym.is(symbol::Type::RESERVED, symbol::PRINTFTK)); // ensured by outer function
	Lexer::getsym();
	assert(sym.is(symbol::Type::DELIM, symbol::LPARENT));
	Lexer::getsym();
    
    int nfield = 0;
	if (sym.is(symbol::Type::STRCON)) {
        nfield++;
        MidCode::gen(MidCode::Instr::OUTPUT_STR, nullptr, nullptr, nullptr, sym.str());
		std::string str = sym.str();
		Lexer::getsym();
		if (sym.is(symbol::Type::DELIM, symbol::COMMA)) {
			Lexer::getsym();
		}
	}
    if (!sym.is(symbol::Type::DELIM, symbol::RPARENT)) {
        nfield++;
        MidCode::gen(MidCode::Instr::OUTPUT_SYM, nullptr, Expr::expr(), nullptr);
    }
    assert(nfield);
    MidCode::gen(MidCode::Instr::OUTPUT_STR, nullptr, nullptr, nullptr, "\\n");
	error::assertSymIsRPARENT();
}

// <ret stat> ::= return['('<expr>')']
void Stat::ret(void) {
	const symtable::Entry* t1 = nullptr;

	assert(sym.is(symbol::Type::RESERVED, symbol::RETURNTK)); // ensured by outer function
	Lexer::getsym();
	if (SymTable::getTable().curFunc().isVoid()) {
		if (sym.is(symbol::Type::DELIM, symbol::LPARENT)) {
			error::raise(error::Code::ILLEGAL_RET_WITH_VAL);
			Lexer::getsym();
			Expr::expr();
			error::assertSymIsRPARENT();
		}
	} else if (sym.is(symbol::Type::DELIM, symbol::LPARENT)) {
		Lexer::getsym();
		t1 = Expr::expr();
		if (SymTable::getTable().curFunc().isInt() != t1->isInt()) {
			error::raise(error::Code::ILLEGAL_RET_WITHOUT_VAL);
		}
		error::assertSymIsRPARENT();
	} else {
		error::raise(error::Code::ILLEGAL_RET_WITHOUT_VAL);
	}
	MidCode::gen(MidCode::Instr::RET, nullptr, t1, nullptr);
}

// <assign> ::= <iden>['['<expr>']']=<expr>
// <iden> is provided by outer function as t0.
void Stat::assign(const symtable::Entry* const t0) {
	const symtable::Entry* t2 = nullptr;
	if (t0->isConst()) { error::raise(error::Code::ILLEGAL_ASSIGN); }
	assert(sym.is(symbol::Type::DELIM));
	if (sym.numIs(symbol::LBRACK)) {
		assert(t0->isInvalid() || t0->isArray());
		Lexer::getsym();
		t2 = Expr::expr();
		if (!t2->isInt()) { error::raise(error::Code::ILLEGAL_IND); }
		error::assertSymIsRBRACK();
	} else {
        assert(t0->isInvalid() || !t0->isArray());
    }

	assert(sym.is(symbol::Type::DELIM, symbol::ASSIGN));
	Lexer::getsym();
	MidCode::gen(t2 == nullptr ? MidCode::Instr::ASSIGN : MidCode::Instr::STORE_IND, 
			t0, Expr::expr(), t2); // t0 = t1[t2];
}

// <stat> ::= <if stat>|<while stat>|<do stat>|<for stat>|'{'{<stat>}'}'|<read stat>;|<write stat>;|<ret stat>;|<assign>;|<func call>;|;
bool Stat::stat(void) {
	bool hasRet = false;
	switch (sym.id()) {
	case symbol::Type::RESERVED:
		switch (sym.num()) {
		case symbol::MAINTK: // calling to main is not exactly a function call
			assert(SymTable::getTable().isMain());
			Lexer::getsym();
			assert(sym.is(symbol::Type::DELIM, symbol::LPARENT));
			Lexer::getsym();
			error::assertSymIsRPARENT();
			MidCode::gen(MidCode::Instr::CALL, nullptr, nullptr, nullptr, "main");
			break;
		case symbol::IFTK: 
			hasRet = Cond::_if();
			break;
		case symbol::WHILETK:
			Cond::_while();
			break;
		case symbol::DOTK:
			hasRet = Cond::_do();
			break;
		case symbol::FORTK:
			Cond::_for();
			break;
		case symbol::SCANFTK:
			read();
			error::assertSymIsSEMICN();
			break;
		case symbol::PRINTFTK:
			write();
			error::assertSymIsSEMICN();
			break;
		case symbol::RETURNTK:
			hasRet = true; // recursion exit
			ret();
			error::assertSymIsSEMICN();
			break;
		default: assert(0);
		}
		break;
	case symbol::Type::IDENFR: {
			std::string name = sym.str();
			Lexer::getsym();
			assert(sym.is(symbol::Type::DELIM));
			if (sym.numIs(symbol::LPARENT)) {
				Func::argValues(SymTable::getTable().findFunc(name));
			} else {
                assign(SymTable::getTable().findSym(name));
            }
			error::assertSymIsSEMICN();
		}
		break;
	case symbol::Type::DELIM:
		switch (sym.num()) {
		case symbol::LBRACE:
			Lexer::getsym();
			while (!sym.is(symbol::Type::DELIM, symbol::RBRACE)) {
				hasRet = stat() || hasRet;
			}
			Lexer::getsym();
			break;
		case symbol::SEMICN:
			Lexer::getsym();
			break;
		default: assert(0);
		}
		break;
	default: assert(0);
	}
	return hasRet;
}

// <block> ::= '{'<const dec><var dec>{<stat>}'}'
void Stat::block(void) {
	assert(sym.is(symbol::Type::DELIM, symbol::LBRACE));
	Lexer::getsym();
	Const::dec();
	Var::dec();

	while (!sym.is(symbol::Type::DELIM, symbol::RBRACE)) { 
		if (stat()) {
            SymTable::getTable().curFunc().setHasRet();
        }
	}

	// function main does not have subsequent symbols
	if (!SymTable::getTable().isMain()) { Lexer::getsym(); }

    const symtable::FuncTable& functable = SymTable::getTable().curFunc();
	if (functable.hasRet()) { return; }
	// for non-void functions, the default <ret> will not fit
	if (!functable.isVoid()) {
        error::raise(error::Code::ILLEGAL_RET_WITHOUT_VAL);
    }
	MidCode::gen(MidCode::Instr::RET, nullptr, nullptr, nullptr);
}

