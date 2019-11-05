/**********************************************
    > File Name: Stat.cpp
    > Author: Luting Wang
    > Mail: 2457348692@qq.com 
    > Created Time: Sat Sep 28 10:43:53 2019
 **********************************************/

#include <cassert>
#include <map>
#include "compiler.h"
#include "basics.h"
#include "Const.h"
#include "Expr.h"
#include "Func.h"
#include "Stat.h"
#include "Var.h"
#include "MidCode.h"
#include "symtable.h"
#include "error.h"
using lexer::getsym;

// <cond> ::= <expr>[<comp op><expr>]
void Stat::Cond::cond(const bool branchIfNot, const std::string& labelName) {
	static std::map<symbol::Comp, MidCode::Instr> translator = {
		{ symbol::LSS, MidCode::Instr::BLT }, { symbol::LEQ, MidCode::Instr::BLE },
		{ symbol::GRE, MidCode::Instr::BGT }, { symbol::GEQ, MidCode::Instr::BGE },
		{ symbol::EQL, MidCode::Instr::BEQ }, { symbol::NEQ, MidCode::Instr::BNE }
	};
	symtable::Entry* t1 = Expr::expr();
	if (!t1->isInt) { err << error::Code::MISMATCHED_COND_TYPE << std::endl; }
	if (sym.is(symbol::Type::COMP)) {
		MidCode::Instr comp = translator[symbol::Comp(sym.num)];
		getsym();
		symtable::Entry* t2 = Expr::expr();
		if (!t2->isInt) { err << error::Code::MISMATCHED_COND_TYPE << std::endl; }
		if (branchIfNot) { std::swap(t1, t2); }
		MidCode(comp, nullptr, t1, t2, labelName);
	} else {
		MidCode(branchIfNot ? MidCode::Instr::BEQ : MidCode::Instr::BNE, nullptr, t1, nullptr, labelName);
	}
}

// <if stat> ::= if'('<cond>')'<stat>[else<stat>]
bool Stat::Cond::_if(void) {
	assert(sym.is(symbol::Type::RESERVED, symbol::IFTK)); // ensured by outer function
	getsym();
	assert(sym.is(symbol::Type::DELIM, symbol::LPARENT));
	getsym();
	std::string labelElse = MidCode::genLabel();
	cond(true, labelElse);
	error::assertSymIsRPARENT();

	bool hasRet = stat();
	if (sym.is(symbol::Type::RESERVED, symbol::ELSETK)) {
		std::string labelEnd = MidCode::genLabel();
		MidCode(MidCode::Instr::GOTO, nullptr, nullptr, nullptr, labelEnd);
		MidCode(MidCode::Instr::LABEL, nullptr, nullptr, nullptr, labelElse);
		getsym();
		hasRet = stat() && hasRet; // ensures that stat() is executed
		MidCode(MidCode::Instr::LABEL, nullptr, nullptr, nullptr, labelEnd);
	} else {
		MidCode(MidCode::Instr::LABEL, nullptr, nullptr, nullptr, labelElse);
	}
	return hasRet;
}

// <while stat> ::= while'('<cond>')'<stat>
bool Stat::Cond::_while(void) {
	assert(sym.is(symbol::Type::RESERVED, symbol::WHILETK)); // ensured by outer function
	getsym();
	assert(sym.is(symbol::Type::DELIM, symbol::LPARENT));
	getsym();
	std::string labelBegin = MidCode::genLabel();
	MidCode(MidCode::Instr::LABEL, nullptr, nullptr, nullptr, labelBegin);
	std::string labelEnd = MidCode::genLabel();
	cond(true, labelEnd);
	error::assertSymIsRPARENT();
	bool hasRet = stat();
	MidCode(MidCode::Instr::GOTO, nullptr, nullptr, nullptr, labelBegin);
	MidCode(MidCode::Instr::LABEL, nullptr, nullptr, nullptr, labelEnd);
	return hasRet;
}

// <do stat> ::= do<stat>while'('<cond>')'
bool Stat::Cond::_do(void) {
	assert(sym.is(symbol::Type::RESERVED, symbol::DOTK)); // ensured by outer function
	getsym();
	std::string labelBegin = MidCode::genLabel();
	MidCode(MidCode::Instr::LABEL, nullptr, nullptr, nullptr, labelBegin);
	bool hasRet = stat();
	if (sym.is(symbol::Type::RESERVED, symbol::WHILETK)) { getsym(); } 
	else { err << error::Code::MISSING_WHILE << std::endl; }
	assert(sym.is(symbol::Type::DELIM, symbol::LPARENT));
	getsym();
	cond(false, labelBegin);
	error::assertSymIsRPARENT();
	return hasRet;
}

// <for stat> ::= for'('<iden>=<expr>;<cond>;<iden>=<iden><add op><unsigned int>')'<stat>
bool Stat::Cond::_for(void) {
	symtable::Entry* t0;
	symtable::Entry* t1;

	// for'('
	assert(sym.is(symbol::Type::RESERVED, symbol::FORTK)); // ensured by outer function
	getsym();
	assert(sym.is(symbol::Type::DELIM, symbol::LPARENT));
	getsym();

	// <iden>=<expr>;
	assert(sym.is(symbol::Type::IDENFR));
	t0 = table.findSym(sym.str);
	if (t0 == nullptr) { err << error::Code::NODEF << std::endl; }
	else if (t0->isConst) { err << error::Code::ILLEGAL_ASSIGN << std::endl; }
	getsym();
	assert(sym.is(symbol::Type::DELIM, symbol::ASSIGN));
	getsym();
	t1 = Expr::expr();
	MidCode(MidCode::Instr::ASSIGN, t0, t1, nullptr);
	error::assertSymIsSEMICN();

	// <cond>;
	std::string labelBegin = MidCode::genLabel();
	MidCode(MidCode::Instr::LABEL, nullptr, nullptr, nullptr, labelBegin);
	std::string labelEnd = MidCode::genLabel();
	cond(true, labelEnd);
	error::assertSymIsSEMICN();

	// <iden>=<iden><add op><unsigned int>')'
	assert(sym.is(symbol::Type::IDENFR));
	t0 = table.findSym(sym.str);
	if (t0 == nullptr) { err << error::Code::NODEF << std::endl; }
	else if (t0->isConst) { err << error::Code::ILLEGAL_ASSIGN << std::endl; }
	getsym();
	assert(sym.is(symbol::Type::DELIM, symbol::ASSIGN));
	getsym();
	assert(sym.is(symbol::Type::IDENFR));
	t1 = table.findSym(sym.str);
	if (t1 == nullptr) { err << error::Code::NODEF << std::endl; }
	getsym();
	bool minus;
	assert(basics::add(minus));
	assert(sym.is(symbol::Type::INTCON));
	symtable::Entry* const stepSize = MidCode::genConst(true, sym.num);
	getsym();
	error::assertSymIsRPARENT();

	// <stat>
	bool hasRet = stat();

	MidCode(minus ? MidCode::Instr::SUB : MidCode::Instr::ADD, t0, t1, stepSize);
	MidCode(MidCode::Instr::GOTO, nullptr, nullptr, nullptr, labelBegin);
	MidCode(MidCode::Instr::LABEL, nullptr, nullptr, nullptr, labelEnd);
	return hasRet;
}

// <read stat> ::= scanf'('<iden>{,<iden>}')'
void Stat::read(void) {
	assert(sym.is(symbol::Type::RESERVED, symbol::SCANFTK)); // ensured by outer function
	getsym();
	assert(sym.is(symbol::Type::DELIM, symbol::LPARENT));
	do {
		getsym();
		assert(sym.is(symbol::Type::IDENFR));
		symtable::Entry* t0 = table.findSym(sym.str);
		if (t0 == nullptr) { err << error::Code::NODEF << std::endl; }
		else if (t0->isConst) { err << error::Code::ILLEGAL_ASSIGN << std::endl; }
		MidCode(MidCode::Instr::INPUT, t0, nullptr, nullptr);
		getsym();
	} while (sym.is(symbol::Type::DELIM, symbol::COMMA));
	error::assertSymIsRPARENT();
}

// <write stat> ::= printf'('<string>[,<expr>]')'|printf'('<expr>')'
void Stat::write(void) {
	assert(sym.is(symbol::Type::RESERVED, symbol::PRINTFTK)); // ensured by outer function
	getsym();
	assert(sym.is(symbol::Type::DELIM, symbol::LPARENT));
	getsym();
	symtable::Entry* t1 = nullptr;
	if (sym.is(symbol::Type::STRCON)) {
		std::string str = sym.str;
		getsym();
		if (sym.is(symbol::Type::DELIM, symbol::COMMA)) {
			getsym();
			t1 = Expr::expr();
		}
		MidCode(MidCode::Instr::OUTPUT, nullptr, t1, nullptr, str);
	} else {
		t1 = Expr::expr();
		MidCode(MidCode::Instr::OUTPUT, nullptr, t1, nullptr);
	}
	error::assertSymIsRPARENT();
}

// <ret stat> ::= return['('<expr>')']
void Stat::ret(void) {
	symtable::Entry* t1 = nullptr;

	assert(sym.is(symbol::Type::RESERVED, symbol::RETURNTK)); // ensured by outer function
	getsym();
	if (table.isMain() || table.curFunc()->isVoid) {
		if (sym.is(symbol::Type::DELIM, symbol::LPARENT)) {
			err << error::Code::ILLEGAL_RET_WITH_VAL << std::endl;
			getsym();
			Expr::expr();
			error::assertSymIsRPARENT();
		}
	} else if (sym.is(symbol::Type::DELIM, symbol::LPARENT)) {
		getsym();
		t1 = Expr::expr();
		if (table.curFunc()->isInt != t1->isInt) {
			err << error::Code::ILLEGAL_RET_WITHOUT_VAL << std::endl;
		}
		error::assertSymIsRPARENT();
	} else {
		err << error::Code::ILLEGAL_RET_WITHOUT_VAL << std::endl;
	}
	MidCode(MidCode::Instr::RET, nullptr, t1, nullptr);
}

// <assign> ::= <iden>['['<expr>']']=<expr>
// <iden> is provided by outer function as t0.
void Stat::assign(symtable::Entry* const t0) {
	symtable::Entry* t2 = nullptr;
	if (t0 == nullptr) { err << error::Code::NODEF << std::endl; }
	else if (t0->isConst) { err << error::Code::ILLEGAL_ASSIGN << std::endl; }
	assert(sym.is(symbol::Type::DELIM));
	if (sym.numIs(symbol::LBRACK)) {
		assert(t0 == nullptr || t0->isArray());
		getsym();
		t2 = Expr::expr();
		if (!t2->isInt) { err << error::Code::ILLEGAL_IND << std::endl; }
		error::assertSymIsRBRACK();
	} else { assert(t0 == nullptr || !t0->isArray()); }

	assert(sym.is(symbol::Type::DELIM, symbol::ASSIGN));
	getsym();
	MidCode(t2 == nullptr ? MidCode::Instr::ASSIGN : MidCode::Instr::STORE_IND, 
			t0, Expr::expr(), t2); // t0 = t1[t2];
}

// <stat> ::= <if stat>|<while stat>|<do stat>|<for stat>|'{'{<stat>}'}'|<read stat>;|<write stat>;|<ret stat>;|<assign>;|<func call>;|;
bool Stat::stat(void) {
	bool hasRet = false;
	switch (sym.id) {
	case symbol::Type::RESERVED:
		switch (sym.num) {
		case symbol::IFTK: 
			hasRet = Cond::_if();
			break;
		case symbol::WHILETK:
			hasRet = Cond::_while();
			break;
		case symbol::DOTK:
			hasRet = Cond::_do();
			break;
		case symbol::FORTK:
			hasRet = Cond::_for();
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
			std::string name = sym.str;
			getsym();
			assert(sym.is(symbol::Type::DELIM));
			if (sym.numIs(symbol::LPARENT)) {
				Func::argValues(table.findFunc(name));
			} else { assign(table.findSym(name)); }
			error::assertSymIsSEMICN();
		}
		break;
	case symbol::Type::DELIM:
		switch (sym.num) {
		case symbol::LBRACE:
			getsym();
			while (!sym.is(symbol::Type::DELIM, symbol::RBRACE)) {
				hasRet = stat() || hasRet;
			}
			// fallthrough
		case symbol::SEMICN:
			getsym();
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
	getsym();
	Const::dec();
	Var::dec();

	while (!sym.is(symbol::Type::DELIM, symbol::RBRACE)) { 
		if (stat()) { table.setHasRet(); }
	}

	// function main does not have subsequent symbols
	if (table.isMain()) { return; }
	
	getsym();
	symtable::FuncTable* ft = table.curFunc();
	if (ft->hasRet()) { return; }
	if (ft->isVoid) {
		MidCode(MidCode::Instr::RET, nullptr, nullptr, nullptr);
	} else {
		// for non-void functions, the default <ret> will not fit
		err << error::Code::ILLEGAL_RET_WITHOUT_VAL << std::endl;
	}
}

