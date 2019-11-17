/**********************************************
    > File Name: grammar.cpp
    > Author: Luting Wang
    > Mail: 2457348692@qq.com 
    > Created Time: Thu Sep 26 21:58:02 2019
 **********************************************/

#include "symtable/table.h"
#include "symtable/SymTable.h"

#include "../include/errors.h"
#include "../include/lexer.h"

#include "Const.h"
#include "Func.h"
#include "Var.h"

#include "frontend.h"

// <program> ::= <const dec><var dec><func dec>
void grammar::parse(void) try {
	lexer::getsym();
	Const::dec();
	Var::dec();
	Func::dec();
    SymTable::getTable().makeConst();
} catch (error::Ueof& e) {
	error::raise(error::Code::UNEXPECTED_EOF);
}

