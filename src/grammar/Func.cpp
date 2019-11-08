/**********************************************
    > File Name: Func.cpp
    > Author: Luting Wang
    > Mail: 2457348692@qq.com 
    > Created Time: Sat Sep 28 10:51:37 2019
 **********************************************/

#include <cassert>
#include <vector>
#include "compiler.h"
#include "error.h"
#include "midcode.h"
#include "symtable.h"

#include "basics.h"
#include "Expr.h"
#include "Stat.h"

#include "Func.h"
using lexer::getsym;

// <args> ::= [<type id><iden>{,<type id><iden>}]
void Func::args(void) {
	bool isInt;
	if (!basics::typeId(isInt)) { return; } // empty is allowed 
	assert(sym.is(symbol::Type::IDENFR));
	table.pushArg(sym.str, isInt);
	for (getsym(); sym.is(symbol::Type::DELIM, symbol::COMMA); getsym()) {
		getsym();
		assert(basics::typeId(isInt));
		assert(sym.is(symbol::Type::IDENFR));
		table.pushArg(sym.str, isInt);
	}
}

// <func def> ::= '('<args>')'<block>
void Func::def(void) {
	assert(sym.is(symbol::Type::DELIM, symbol::LPARENT));
	getsym();
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
			getsym();
			if (!sym.is(symbol::Type::IDENFR)) { break; }
			table.pushFunc(sym.str);
		} else {
			assert(sym.is(symbol::Type::IDENFR));
			table.pushFunc(sym.str, isInt);
		}
		getsym();
		def();
	}
	assert(sym.is(symbol::Type::RESERVED, symbol::MAINTK));
	table.pushFunc();
	getsym();
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
symtable::Entry* Func::argValues(const symtable::FuncTable* const ft) { 
	assert(sym.is(symbol::Type::DELIM, symbol::LPARENT)); // ensured by outer function
	getsym();

	std::vector<symtable::Entry*> argv;
	if (!sym.is(symbol::Type::DELIM, symbol::RPARENT|symbol::SEMICN)) { // ')' might be missing
		while (true) {
			argv.push_back(Expr::expr());
			if (!sym.is(symbol::Type::DELIM, symbol::COMMA)) { break; }
			getsym();
		}
	}
	error::assertSymIsRPARENT();

	if (ft == nullptr) { 
		error::raise(error::Code::NODEF); 
		return nullptr;
	}

	// check and push args
	const std::vector<symtable::Entry*>& al = ft->argList();
	if (argv.size() != al.size()) {
		error::raise(error::Code::MISMATCHED_ARG_NUM);
	} else for (int i = 0; i < argv.size(); i++) {
		assert(!al[i]->isConst); // ensured by symtable
		if (argv[i]->isInt != al[i]->isInt) {
			error::raise(error::Code::MISMATCHED_ARG_TYPE);
			break;
		}
		MidCode::gen(MidCode::Instr::PUSH_ARG, nullptr, argv[i], nullptr);
	}

	// generate mid code
	symtable::Entry* t0 = ft->isVoid ? nullptr : MidCode::genVar(ft->isInt);
	MidCode::gen(MidCode::Instr::CALL, t0, nullptr, nullptr, ft->name());
	return t0;
}

