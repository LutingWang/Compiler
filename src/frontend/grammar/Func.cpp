/**********************************************
    > File Name: Func.cpp
    > Author: Luting Wang
    > Mail: 2457348692@qq.com 
    > Created Time: Sat Sep 28 10:51:37 2019
 **********************************************/

#include <cassert>
#include <vector>
#include "midcode/MidCode.h"
#include "symtable/table.h"
#include "symtable/Entry.h"
#include "symtable/SymTable.h"

#include "../include/errors.h"
#include "../include/Lexer.h"

#include "basics.h"
#include "Expr.h"
#include "Stat.h"

#include "Func.h"

// <args> ::= [<type id><iden>{,<type id><iden>}]
void Func::args(void) {
	bool isInt;
	if (!basics::typeId(isInt)) { return; } // empty is allowed 
	assert(sym.is(symbol::Type::IDENFR));
	SymTable::getTable().curFunc().pushArg(sym.str(), isInt);
	for (Lexer::getsym(); sym.is(symbol::Type::DELIM, symbol::COMMA); Lexer::getsym()) {
		Lexer::getsym();
		assert(basics::typeId(isInt));
		assert(sym.is(symbol::Type::IDENFR));
		SymTable::getTable().curFunc().pushArg(sym.str(), isInt);
	}
}

// <func def> ::= '('<args>')'<block>
void Func::def(void) {
	assert(sym.is(symbol::Type::DELIM, symbol::LPARENT));
	Lexer::getsym();
	args();
	error::assertSymIsRPARENT();
	Stat::block();
}

// <func dec> ::= {(<type id>|void)<iden><func def>}void main<func def>
void Func::dec(void) {
	bool isInt;
	while (true) {
		assert(sym.is(symbol::Type::RESERVED));
		if (!basics::typeId(isInt)) {
			assert(sym.numIs(symbol::VOIDTK));
			Lexer::getsym();
			if (!sym.is(symbol::Type::IDENFR)) { break; }
			SymTable::getTable().pushFunc(sym.str());
		} else {
			assert(sym.is(symbol::Type::IDENFR));
			SymTable::getTable().pushFunc(sym.str(), isInt);
		}
		Lexer::getsym();
		def();
	}
	assert(sym.is(symbol::Type::RESERVED, symbol::MAINTK));
	SymTable::getTable().pushFunc();
	Lexer::getsym();
	def();
}

// <arg values> ::= '('[<expr>{,<expr>}]')'
// input : information on the corresponding function
// output : mid code target t0
// 
// Output is set to null only when function is void. Otherwise, 
// the `isInt` field of return value indicates whether the 
// original function was int.
//
// This function is obligated to check whether the arg values
// match with the function declaration.
const symtable::Entry* Func::argValues(const symtable::FuncTable* const functable) { 
	assert(sym.is(symbol::Type::DELIM, symbol::LPARENT)); // ensured by outer function
	Lexer::getsym();

	std::vector<const symtable::Entry*> argv;
	if (!sym.is(symbol::Type::DELIM, symbol::RPARENT|symbol::SEMICN)) { // ')' might be missing
		while (true) {
			argv.push_back(Expr::expr());
			if (!sym.is(symbol::Type::DELIM, symbol::COMMA)) { break; }
			Lexer::getsym();
		}
	}
	error::assertSymIsRPARENT();

	if (functable == nullptr) { 
		return nullptr;
	}

	// check and push args
	const std::vector<const symtable::Entry*>& arglist = functable->argList();
	if (argv.size() != arglist.size()) {
		error::raise(error::Code::MISMATCHED_ARG_NUM);
	} else for (int i = 0; i < argv.size(); i++) {
		assert(!arglist[i]->isConst()); // ensured by symtable
		if (argv[i]->isInt() != arglist[i]->isInt()) {
			error::raise(error::Code::MISMATCHED_ARG_TYPE);
			break;
		}
		MidCode::gen(MidCode::Instr::PUSH_ARG, nullptr, argv[i], nullptr);
	}

	// generate mid code
	const symtable::Entry* const t0 = functable->isVoid() ? nullptr : MidCode::genVar(functable->isInt());
	MidCode::gen(MidCode::Instr::CALL, t0, nullptr, nullptr, functable->name());
	return t0;
}

