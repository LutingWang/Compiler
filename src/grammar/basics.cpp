/**********************************************
    > File Name: basics.cpp
    > Author: Luting Wang
    > Mail: 2457348692@qq.com 
    > Created Time: Mon Sep 30 08:18:53 2019
 **********************************************/

#include "compiler.h"
#include "Symbol.h"

#include "basics.h"
using lexer::getsym;

namespace {
	// template para : type of desired and oppo symbol
	// input : 
	//		- result : inout parameter indicating whether symbol is desired
	//		- desired : desired symbol
	//		- oppo : undesired symbol
	// output : whether the symbol is either desired or oppo
	template<const symbol::Type type>
	bool _template(bool& result, unsigned int desired, unsigned int oppo) {
		if (!sym.is(type)) { 
			result = false;
			return false; 
		}
		result = sym.numIs(desired);
		if (result || sym.numIs(oppo)) {
			getsym();
			return true;
		} else { return false; }
	}
}

// <add op> ::= +|-
bool basics::add(bool& isNeg) {
	return _template<symbol::Type::OPER>(isNeg, symbol::MINU, symbol::PLUS);
}

// <mult op> ::= *|/
bool basics::mult(bool& isMult) {
	return _template<symbol::Type::OPER>(isMult, symbol::MULT, symbol::DIV);
}

// <type id> ::= int|char
bool basics::typeId(bool& isInt) {
	return _template<symbol::Type::RESERVED>(isInt, symbol::INTTK, symbol::CHARTK);
}
