/**********************************************
    > File Name: Stat.cpp
    > Author: Luting Wang
    > Mail: 2457348692@qq.com 
    > Created Time: Sat Sep 28 10:43:53 2019
 **********************************************/

#include <cassert>
#include "compiler.h"
#include "basics.h"
#include "Const.h"
#include "Expr.h"
#include "Func.h"
#include "Stat.h"
#include "Var.h"
#include "symtable.h"
#include "error.h"
using lexer::getsym;

// <cond> ::= <expr>[<comp op><expr>]
void Stat::Cond::cond(void) {
	if (!Expr::expr()) { err << error::MISMATCHED_COND_TYPE << std::endl; }
	if (sym.is(symbol::COMP)) {
		getsym();
		if (!Expr::expr()) { err << error::MISMATCHED_COND_TYPE << std::endl; }
	}
}

// <if stat> ::= if'('<cond>')'<stat>[else<stat>]
bool Stat::Cond::_if(void) {
	assert(sym.is(symbol::RESERVED, symbol::IFTK)); // ensured by outer function
	getsym();
	assert(sym.is(symbol::DELIM, symbol::LPARENT));
	getsym();
	cond();
	error::assertSymIsRPARENT();

	bool hasRet = stat();
	if (sym.is(symbol::RESERVED, symbol::ELSETK)) {
		getsym();
		hasRet = stat() && hasRet; // ensures that stat() is executed
	}
	return hasRet;
}

// <while stat> ::= while'('<cond>')'<stat>
bool Stat::Cond::_while(void) {
	assert(sym.is(symbol::RESERVED, symbol::WHILETK)); // ensured by outer function
	getsym();
	assert(sym.is(symbol::DELIM, symbol::LPARENT));
	getsym();
	cond();
	error::assertSymIsRPARENT();
	return stat();
}

// <do stat> ::= do<stat>while'('<cond>')'
bool Stat::Cond::_do(void) {
	assert(sym.is(symbol::RESERVED, symbol::DOTK)); // ensured by outer function
	getsym();
	bool hasRet = stat();
	if (sym.is(symbol::RESERVED, symbol::WHILETK)) { getsym(); } 
	else { err << error::MISSING_WHILE << std::endl; }
	assert(sym.is(symbol::DELIM, symbol::LPARENT));
	getsym();
	cond();
	error::assertSymIsRPARENT();
	return hasRet;
}

// <for stat> ::= for'('<iden>=<expr>;<cond>;<iden>=<iden><add op><unsigned int>')'<stat>
bool Stat::Cond::_for(void) {
	assert(sym.is(symbol::RESERVED, symbol::FORTK)); // ensured by outer function
	getsym();
	assert(sym.is(symbol::DELIM, symbol::LPARENT));
	getsym();
	const symtable::Entry* entry;
	assert(sym.is(symbol::IDENFR));
	entry = table.findSym(sym.str);
	if (entry == nullptr) { err << error::NODEF << std::endl; }
	else if (entry->isConst) { err << error::ILLEGAL_ASSIGN << std::endl; }
	getsym();
	assert(sym.is(symbol::DELIM, symbol::ASSIGN));
	getsym();
	Expr::expr();
	error::assertSymIsSEMICN();
	cond();
	error::assertSymIsSEMICN();
	assert(sym.is(symbol::IDENFR));
	entry = table.findSym(sym.str);
	if (entry == nullptr) { err << error::NODEF << std::endl; }
	else if (entry->isConst) { err << error::ILLEGAL_ASSIGN << std::endl; }
	getsym();
	assert(sym.is(symbol::DELIM, symbol::ASSIGN));
	getsym();
	assert(sym.is(symbol::IDENFR));
	entry = table.findSym(sym.str);
	if (entry == nullptr) { err << error::NODEF << std::endl; }
	getsym();
	bool minus;
	assert(basics::add(minus));
	assert(sym.is(symbol::INTCON));
	getsym();
	error::assertSymIsRPARENT();
	return stat();
}

// <read stat> ::= scanf'('<iden>{,<iden>}')'
void Stat::read(void) {
	assert(sym.is(symbol::RESERVED, symbol::SCANFTK)); // ensured by outer function
	getsym();
	assert(sym.is(symbol::DELIM, symbol::LPARENT));
	const symtable::Entry* entry;
	do {
		getsym();
		assert(sym.is(symbol::IDENFR));
		entry = table.findSym(sym.str);
		if (entry == nullptr) { err << error::NODEF << std::endl; }
		else if (entry->isConst) { err << error::ILLEGAL_ASSIGN << std::endl; }
		getsym();
	} while (sym.is(symbol::DELIM, symbol::COMMA));
	error::assertSymIsRPARENT();
}

// <write stat> ::= printf'('<string>[,<expr>]')'|printf'('<expr>')'
void Stat::write(void) {
	assert(sym.is(symbol::RESERVED, symbol::PRINTFTK)); // ensured by outer function
	getsym();
	assert(sym.is(symbol::DELIM, symbol::LPARENT));
	getsym();
	if (sym.is(symbol::STRCON)) {
		getsym();
		if (sym.is(symbol::DELIM, symbol::COMMA)) {
			getsym();
			Expr::expr();
		}
	} else {
		Expr::expr();
	}
	error::assertSymIsRPARENT();
}

// <ret stat> ::= return['('<expr>')']
void Stat::ret(void) {
	assert(sym.is(symbol::RESERVED, symbol::RETURNTK)); // ensured by outer function
	getsym();
	if (table.isMain() || table.curFunc()->isVoid) {
		if (sym.is(symbol::DELIM, symbol::LPARENT)) {
			err << error::ILLEGAL_RET_WITH_VAL << std::endl;
			getsym();
			Expr::expr();
			error::assertSymIsRPARENT();
		}
	} else if (sym.is(symbol::DELIM, symbol::LPARENT)) {
		getsym();
		bool isInt = Expr::expr();
		if (table.curFunc()->isInt != isInt) {
			err << error::ILLEGAL_RET_WITHOUT_VAL << std::endl;
		}
		error::assertSymIsRPARENT();
	} else {
		err << error::ILLEGAL_RET_WITHOUT_VAL << std::endl;
	}
}

// <assign> ::= <iden>['['<expr>']']=<expr>
// <iden> is provided by outer function in the form of `Entry*`.
void Stat::assign(const symtable::Entry* entry) {
	if (entry == nullptr) { err << error::NODEF << std::endl; }
	else if (entry->isConst) { err << error::ILLEGAL_ASSIGN << std::endl; }
	assert(sym.is(symbol::DELIM));
	switch (sym.num) {
	case symbol::LBRACK:
		getsym();
		if (!Expr::expr()) { err << error::ILLEGAL_IND << std::endl; }
		error::assertSymIsRBRACK();
		assert(sym.is(symbol::DELIM, symbol::ASSIGN));
	case symbol::ASSIGN:
		getsym();
		Expr::expr();
		break;
	default: assert(0);
	}
}

// <stat> ::= <if stat>|<while stat>|<do stat>|<for stat>|'{'{<stat>}'}'|<read stat>;|<write stat>;|<ret stat>;|<assign>;|<func call>;|;
bool Stat::stat(void) {
	bool hasRet = false;
	switch (sym.id) {
	case symbol::RESERVED:
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
	case symbol::IDENFR: {
			std::string name = sym.str;
			getsym();
			assert(sym.is(symbol::DELIM));
			if (sym.numIs(symbol::LPARENT)) {
				Func::argValues(table.findFunc(name));
			} else { assign(table.findSym(name)); }
			error::assertSymIsSEMICN();
		}
		break;
	case symbol::DELIM:
		switch (sym.num) {
		case symbol::LBRACE:
			getsym();
			while (!sym.is(symbol::DELIM, symbol::RBRACE)) {
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
	assert(sym.is(symbol::DELIM, symbol::LBRACE));
	getsym();
	Const::dec();
	Var::dec();

	bool hasRet = false;
	while (!sym.is(symbol::DELIM, symbol::RBRACE)) { 
		hasRet = stat() || hasRet;
	}

	// function main does not have subsequent symbols
	if (!table.isMain()) {
		// for non-void functions, the default <ret> will not fit
		if (!table.curFunc()->isVoid && !hasRet) {
			err << error::ILLEGAL_RET_WITHOUT_VAL << std::endl;
		}
		getsym();
	}

}

