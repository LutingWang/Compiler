/**********************************************
    > File Name: Stat.cpp
    > Author: Luting Wang
    > Mail: 2457348692@qq.com 
    > Created Time: Sat Sep 28 10:43:53 2019
 **********************************************/

#include <cassert>
#include "basics.h"
#include "Const.h"
#include "Expr.h"
#include "Func.h"
#include "Stat.h"
#include "Var.h"
#include "lexer.h"
using lexer::getsym;

#include "debug.h" // <>

// <cond> ::= <expr>[<comp op><expr>]
void Stat::Cond::cond(void) {
	Expr::expr();
	if (sym.id == lexer::COMP) {
		getsym();
		Expr::expr();
	}
	std::string s = v.back(); // <
	v.pop_back();
	v.push_back(print(CONDITION));
	v.push_back(s); // >
}

// <if stat> ::= if'('<cond>')'<stat>[else<stat>]
void Stat::Cond::_if(void) {
	assert(sym.id == lexer::RESERVED && sym.num == lexer::IFTK); // ensured by outer function
	getsym();
	assert(sym.id == lexer::DELIM && sym.num == lexer::LPARENT);
	getsym();
	cond();
	assert(sym.id == lexer::DELIM && sym.num == lexer::RPARENT);
	getsym();
	stat();
	if (sym.id == lexer::RESERVED && sym.num == lexer::ELSETK) {
		getsym();
		stat();
	}
	std::string s = v.back(); // <
	v.pop_back();
	v.push_back(print(COND));
	v.push_back(s); // >
}

// <while stat> ::= while'('<cond>')'<stat>
void Stat::Cond::_while(void) {
	assert(sym.id == lexer::RESERVED && sym.num == lexer::WHILETK); // ensured by outer function
	getsym();
	assert(sym.id == lexer::DELIM && sym.num == lexer::LPARENT);
	getsym();
	cond();
	assert(sym.id == lexer::DELIM && sym.num == lexer::RPARENT);
	getsym();
	stat();
	std::string s = v.back(); // <
	v.pop_back();
	v.push_back(print(ITER));
	v.push_back(s); // >
}

// <do stat> ::= do<stat>while'('<cond>')'
void Stat::Cond::_do(void) {
	assert(sym.id == lexer::RESERVED && sym.num == lexer::DOTK); // ensured by outer function
	getsym();
	stat();
	assert(sym.id == lexer::RESERVED && sym.num == lexer::WHILETK); 
	getsym();
	assert(sym.id == lexer::DELIM && sym.num == lexer::LPARENT);
	getsym();
	cond();
	assert(sym.id == lexer::DELIM && sym.num == lexer::RPARENT);
	v.push_back(print(ITER)); // <>
	getsym();
}

// <for stat> ::= for'('<iden>=<expr>;<cond>;<iden>=<iden><add op><unsigned int>')'<stat>
void Stat::Cond::_for(void) {
	assert(sym.id == lexer::RESERVED && sym.num == lexer::FORTK); // ensured by outer function
	getsym();
	assert(sym.id == lexer::DELIM && sym.num == lexer::LPARENT);
	getsym();
	assert(sym.id == lexer::IDENFR);
	getsym();
	assert(sym.id == lexer::DELIM && sym.num == lexer::ASSIGN);
	getsym();
	Expr::expr();
	assert(sym.id == lexer::DELIM && sym.num == lexer::SEMICN);
	getsym();
	cond();
	assert(sym.id == lexer::DELIM && sym.num == lexer::SEMICN);
	getsym();
	assert(sym.id == lexer::IDENFR);
	getsym();
	assert(sym.id == lexer::DELIM && sym.num == lexer::ASSIGN);
	getsym();
	assert(sym.id == lexer::IDENFR);
	getsym();
	bool minus;
	assert(basics::add(minus));
	assert(sym.id == lexer::INTCON);
	v.push_back(print(STEP)); // <>
	getsym();
	assert(sym.id == lexer::DELIM && sym.num == lexer::RPARENT);
	getsym();
	stat();
	std::string s = v.back(); // <
	v.pop_back();
	v.push_back(print(ITER));
	v.push_back(s); // >
}

// <read stat> ::= scanf'('<iden>{,<iden>}')'
void Stat::read(void) {
	assert(sym.id == lexer::RESERVED && sym.num == lexer::SCANFTK); // ensured by outer function
	getsym();
	assert(sym.id == lexer::DELIM && sym.num == lexer::LPARENT);
	getsym();
	assert(sym.id == lexer::IDENFR);
	for (getsym(); sym.id == lexer::DELIM && sym.num == lexer::COMMA; getsym()) {
		getsym();
		assert(sym.id == lexer::IDENFR);
	}
	assert(sym.id == lexer::DELIM && sym.num == lexer::RPARENT);
	v.push_back(print(READ)); // <>
	getsym();
}

// <write stat> ::= printf'('<string>[,<expr>]')'|printf'('<expr>')'
void Stat::write(void) {
	assert(sym.id == lexer::RESERVED && sym.num == lexer::PRINTFTK); // ensured by outer function
	getsym();
	assert(sym.id == lexer::DELIM && sym.num == lexer::LPARENT);
	getsym();
	if (sym.id == lexer::STRCON) {
		getsym();
		if (sym.id == lexer::DELIM && sym.num == lexer::COMMA) {
			getsym();
			Expr::expr();
		}
	} else {
		Expr::expr();
	}
	assert(sym.id == lexer::DELIM && sym.num == lexer::RPARENT);
	v.push_back(print(WRITE)); // <>
	getsym();
}

// <ret stat> ::= return['('<expr>')']
void Stat::ret(void) {
	assert(sym.id == lexer::RESERVED && sym.num == lexer::RETURNTK); // ensured by outer function
	getsym();
	if (sym.id == lexer::DELIM && sym.num == lexer::LPARENT) {
		getsym();
		Expr::expr();
		assert(sym.id == lexer::DELIM && sym.num == lexer::RPARENT);
		getsym();
	}
	std::string s = v.back(); // <
	v.pop_back();
	v.push_back(print(RET));
	v.push_back(s); // >
}

// <assign> ::= <iden>['['<expr>']']=<expr>
void Stat::assign(void) {
	assert(sym.id == lexer::IDENFR); // ensured by outer function
	getsym();
	assert(sym.id == lexer::DELIM);
	switch (sym.num) {
	case lexer::LBRACK:
		getsym();
		Expr::expr();
		assert(sym.id == lexer::DELIM && sym.num == lexer::RBRACK);
		getsym();
		assert(sym.id == lexer::DELIM && sym.num == lexer::ASSIGN);
	case lexer::ASSIGN:
		getsym();
		Expr::expr();
		break;
	default: assert(0);
	}
	std::string s = v.back(); // <
	v.pop_back();
	v.push_back(print(ASSIGN));
	v.push_back(s); // >
}

// <stat> ::= <if stat>|<while stat>|<do stat>|<for stat>|'{'{<stat>}'}'|<read stat>;|<write stat>;|<ret stat>;|<assign>;|<func call>;|;
void Stat::stat(void) {
	switch (sym.id) {
	case lexer::RESERVED:
		switch (sym.num) {
		case lexer::IFTK: 
			Cond::_if();
			break;
		case lexer::WHILETK:
			Cond::_while();
			break;
		case lexer::DOTK:
			Cond::_do();
			break;
		case lexer::FORTK:
			Cond::_for();
			break;
		case lexer::SCANFTK:
			read();
			assert(sym.id == lexer::DELIM && sym.num == lexer::SEMICN);
			getsym();
			break;
		case lexer::PRINTFTK:
			write();
			assert(sym.id == lexer::DELIM && sym.num == lexer::SEMICN);
			getsym();
			break;
		case lexer::RETURNTK:
			ret();
			assert(sym.id == lexer::DELIM && sym.num == lexer::SEMICN);
			getsym();
			break;
		default: assert(0);
		}
		break;
	case lexer::IDENFR: {
			lexer::Symbol lastSymbol = sym;
			getsym();
			assert(sym.id == lexer::DELIM);
			bool isFuncCall = sym.num == lexer::LPARENT;
			lexer::traceback(lastSymbol);
			if (isFuncCall) { Func::call(); }
			else { assign(); }
			assert(sym.id == lexer::DELIM && sym.num == lexer::SEMICN);
			getsym();
		}
		break;
	case lexer::DELIM:
		switch (sym.num) {
		case lexer::LBRACE:
			for (getsym(); sym.id != lexer::DELIM || sym.num != lexer::RBRACE;) {
				stat();
			}
			{ // <
				std::string s = v.back(); 
				v.pop_back();
				v.push_back(print(STAT_SERIES));
				v.push_back(s); 
			} // >
			// fallthrough
		case lexer::SEMICN:
			getsym();
			break;
		default: assert(0);
		}
		break;
	default: assert(0);
	}
	std::string s = v.back(); // <
	v.pop_back();
	v.push_back(print(STAT));
	v.push_back(s); // >
}

// <block> ::= '{'<const dec><var dec>{<stat>}'}'
void Stat::block(void) {
	assert(sym.id == lexer::DELIM && sym.num == lexer::LBRACE);
	getsym();
	Const::dec();
	Var::dec();
	while (sym.id != lexer::DELIM || sym.num != lexer::RBRACE) {
		stat();
	}
	std::string s = v.back(); // <
	v.pop_back();
	v.push_back(print(STAT_SERIES));
	v.push_back(print(BLOCK));
	v.push_back(s); // >
	getsym();
}

