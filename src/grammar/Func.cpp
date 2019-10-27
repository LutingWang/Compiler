/**********************************************
    > File Name: Func.cpp
    > Author: Luting Wang
    > Mail: 2457348692@qq.com 
    > Created Time: Sat Sep 28 10:51:37 2019
 **********************************************/

#include <cassert>
#include <vector>
#include "compiler.h"
#include "basics.h"
#include "Expr.h"
#include "Func.h"
#include "Stat.h"
#include "symtable.h"
#include "error.h"
using lexer::getsym;

// <args> ::= [<type id><iden>{,<type id><iden>}]
void Func::args(void) {
	bool isInt;
	if (!basics::typeId(isInt)) { return; } // empty is allowed 
	assert(sym.is(symbol::IDENFR));
	table.pushArg(sym.str, isInt);
	for (getsym(); sym.is(symbol::DELIM, symbol::COMMA); getsym()) {
		getsym();
		assert(basics::typeId(isInt));
		assert(sym.is(symbol::IDENFR));
		table.pushArg(sym.str, isInt);
	}
}

// <func def> ::= '('<args>')'<block>
void Func::def(void) {
	assert(sym.is(symbol::DELIM, symbol::LPARENT));
	getsym();
	args();
	error::assertSymIsRPARENT();
	Stat::block();
}

// <func dec> ::= {(<type id>|void)<iden><func def>}void main<func def>
void Func::dec(void) {
	bool isInt;
	while (true) {
		assert(sym.is(symbol::RESERVED));
		if (!basics::typeId(isInt)) {
			assert(sym.numIs(symbol::VOIDTK));
			getsym();
			if (!sym.is(symbol::IDENFR)) { break; }
			table.pushFunc(sym.str);
		} else {
			assert(sym.is(symbol::IDENFR));
			table.pushFunc(sym.str, isInt);
		}
		getsym();
		def();
	}
	assert(sym.is(symbol::RESERVED, symbol::MAINTK));
	table.pushFunc();
	getsym();
	def();
}

// <arg values> ::= '('[<expr>{,<expr>}]')'
// input : information on the corresponding function
//
// This function is obligated to check whether the arg values
// match with the function declaration.
void Func::argValues(const symtable::FuncTable* ft) { 
	if (ft == nullptr) { err << error::NODEF << std::endl; }
	assert(sym.is(symbol::DELIM, symbol::LPARENT)); // ensured by outer function
	getsym();

	std::vector<bool> argv;
	if (!sym.is(symbol::DELIM, symbol::RPARENT|symbol::SEMICN)) { // ')' might be missing
		while (true) {
			argv.push_back(Expr::expr());
			if (!sym.is(symbol::DELIM, symbol::COMMA)) { break; }
			getsym();
		}
	}
	error::assertSymIsRPARENT();

	// error handling
	if (ft == nullptr) { return; }
	const std::vector<const symtable::Entry*>& al = ft->argList();
	if (argv.size() != al.size()) {
		err << error::MISMATCHED_ARG_NUM << std::endl;
	} else for (int i = 0; i < argv.size(); i++) {
		assert(!al[i]->isConst); // ensured by symtable
		if (argv[i] != al[i]->isInt) {
			err << error::MISMATCHED_ARG_TYPE << std::endl;
			break;
		}
	}
}

