/**********************************************
    > File Name: grammar.cpp
    > Author: Luting Wang
    > Mail: 2457348692@qq.com 
    > Created Time: Thu Sep 26 21:58:02 2019
 **********************************************/

#include "compiler.h"
#include "Const.h"
#include "Func.h"
#include "Var.h"
#include "error.h"

// <program> ::= <const dec><var dec><func dec>
void grammar::parse(void) try {
	lexer::getsym();
	Const::dec();
	Var::dec();
	Func::dec();
} catch (error::Ueof& e) {
	err << error::Code::UNEXPECTED_EOF << std::endl;
}

