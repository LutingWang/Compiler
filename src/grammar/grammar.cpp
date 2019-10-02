/**********************************************
    > File Name: grammar.cpp
    > Author: Luting Wang
    > Mail: 2457348692@qq.com 
    > Created Time: Thu Sep 26 21:58:02 2019
 **********************************************/

#include "Const.h"
#include "Func.h"
#include "grammar.h"
#include "Var.h"
#include "lexer.h"

#include "debug.h" // <>

// <program> ::= <const dec><var dec><func dec>
void grammar::parse(void) {
	lexer::getsym();
	Const::dec();
	Var::dec();
	Func::dec();
	v.push_back(print(PROGRAM));
}

