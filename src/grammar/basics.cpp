/**********************************************
    > File Name: basics.cpp
    > Author: Luting Wang
    > Mail: 2457348692@qq.com 
    > Created Time: Mon Sep 30 08:18:53 2019
 **********************************************/

#include "basics.h"
#include "lexer.h"
using lexer::getsym;

namespace {
	// template para : type of desired and oppo symbol
	// input : 
	//		- result : inout parameter indicating whether symbol is desired
	//		- desired : desired symbol
	//		- oppo : undesired symbol
	// output : whether the symbol is desired or oppo
	template<const lexer::Type type>
	bool _template(bool& result, unsigned int desired, unsigned int oppo) {
		if (sym.id != type) { 
			result = false;
			return false; 
		}
		result = sym.num == desired;
		if (result || sym.num == oppo) {
			getsym();
			return true;
		} else {
			return false;
		}
	}
}

// <add op> ::= +|-
bool basics::add(bool& isNeg) {
	return _template<lexer::OPER>(isNeg, lexer::MINU, lexer::PLUS);
}

// <mult op> ::= *|/
bool basics::mult(bool& isMult) {
	return _template<lexer::OPER>(isMult, lexer::MULT, lexer::DIV);
}

// <type id> ::= int|char
bool basics::typeId(bool& isInt) {
	return _template<lexer::RESERVED>(isInt, lexer::INTTK, lexer::CHARTK);
}
