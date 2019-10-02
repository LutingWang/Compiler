/**********************************************
    > File Name: Func.cpp
    > Author: Luting Wang
    > Mail: 2457348692@qq.com 
    > Created Time: Sat Sep 28 10:51:37 2019
 **********************************************/

#include <cassert>
#include "basics.h"
#include "Expr.h"
#include "Func.h"
#include "Stat.h"
#include "lexer.h"
#include "symtable.h"
using lexer::getsym;

#include "debug.h" // <>

// <args> ::= [<type id><iden>{,<type id><iden>}]
void Func::args(void) {
	bool isInt;
	if (!basics::typeId(isInt)) { // empty is allowed
		std::string s = v.back(); // <
		v.pop_back();
		v.push_back(print(ARGS));
		v.push_back(s); // >
		return; 
	} 
	assert(sym.id == lexer::IDENFR);
	table.pushSym(sym.str, symtable::ARG, isInt);
	for (getsym(); sym.id == lexer::DELIM && sym.num == lexer::COMMA; getsym()) {
		getsym();
		assert(basics::typeId(isInt));
		assert(sym.id == lexer::IDENFR);
		table.pushSym(sym.str, symtable::ARG, isInt);
	}
	std::string s = v.back(); // <
	v.pop_back();
	v.push_back(print(ARGS));
	v.push_back(s); // >
}

// <func def> ::= <iden>'('<args>')'<multi stat>
// output : whether function is main
bool Func::def(const unsigned int /* lexer::Reserved */ typeId) {
	bool isMain = false;
	if (sym.id == lexer::RESERVED && sym.num == lexer::MAINTK && typeId == lexer::VOIDTK) {
		isMain = true;
		table.pushFunc();
	} else {
		assert(sym.id == lexer::IDENFR);
		switch (typeId) {
		case lexer::VOIDTK:
			table.pushFunc(sym.str, symtable::VOID);
			break;
		case lexer::INTTK:
			table.pushFunc(sym.str, symtable::INT);
			v.push_back(print(DEC_HEAD)); // <>
			break;
		case lexer::CHARTK:
			table.pushFunc(sym.str, symtable::CHAR);
			v.push_back(print(DEC_HEAD)); // <>
			break;
		default:
			assert(0);
		}
	}
	getsym();
	assert(sym.id == lexer::DELIM && sym.num == lexer::LPARENT);
	getsym();
	if (!isMain) args();
	assert(sym.id == lexer::DELIM && sym.num == lexer::RPARENT);
	getsym();
	Stat::block();
	if (!isMain) { // <
		std::string s = v.back(); 
		v.pop_back();
		if (typeId == lexer::VOIDTK) v.push_back(print(FUNC_DEF_WITHOUT_RET));
		else v.push_back(print(FUNC_DEF_WITH_RET));
		v.push_back(s); 
	} // >
	return isMain;
}

// <func dec> ::= {(<type id>|void)<func def>}void<func def: main>
void Func::dec(void) {
	unsigned int typeId;
	do {
		assert(sym.id == lexer::RESERVED);
		typeId = sym.num;
		getsym();
	} while (!def(typeId));
	v.push_back(print(MAIN_FUNC)); // <>
}

// <func call> ::= <iden>'('[<expr>{,<expr>}]')'
void Func::call(void) { 
	assert(sym.id == lexer::IDENFR); // ensured by outer function
	symtable::FuncTable* ft = table.findFunc(sym.str);
	assert(ft != nullptr);
	getsym(); 
	assert(sym.id == lexer::DELIM && sym.num == lexer::LPARENT); // ensured by outer function
	getsym();
	if (sym.id == lexer::DELIM && sym.num == lexer::RPARENT) {
		std::string s = v.back(); // <
		v.pop_back();
		v.push_back(print(VALUES));
		v.push_back(s); 
		v.push_back(print(ft->type != symtable::VOID ? FUNC_CALL_WITH_RET : FUNC_CALL_WITHOUT_RET)); // >
		getsym();
		return;
	}
	Expr::expr();
	while (sym.id == lexer::DELIM && sym.num == lexer::COMMA) {
		getsym();
		Expr::expr();
	}
	std::string s = v.back(); // <
	v.pop_back();
	v.push_back(print(VALUES));
	v.push_back(s); 
	v.push_back(print(ft->type != symtable::VOID ? FUNC_CALL_WITH_RET : FUNC_CALL_WITHOUT_RET)); // >
	assert(sym.id == lexer::DELIM && sym.num == lexer::RPARENT);
	getsym();
}

