/**********************************************
    > File Name: error.h
    > Author: Luting Wang
    > Mail: 2457348692@qq.com 
    > Created Time: Wed Oct  2 21:29:35 2019
 **********************************************/

#ifndef ERROR_H
#define ERROR_H

#include <exception>
#include <cassert>
#include <string>
#include "compiler.h"
#include "symtable.h"

namespace error {
	enum Code {
		LEX, REDEF, NODEF,
		MISMATCHED_ARG_NUM, MISMATCHED_ARG_TYPE, MISMATCHED_COND_TYPE,
		ILLEGAL_RET_WITH_VAL, ILLEGAL_RET_WITHOUT_VAL, ILLEGAL_IND, ILLEGAL_ASSIGN, 
		MISSING_SEMICN, MISSING_RPARENT, MISSING_RBRACK, MISSING_WHILE, 
		EXPECTED_LITERAL, UNEXPECTED_EOF
	};

	std::ostream& operator << (std::ostream&, Code);

	class Ueof : public std::exception {};

	namespace {
#ifdef ASSERT_DELIM
	#error macro conflict
#endif
#define ASSERT_DELIM(s) inline void assertSymIs##s(void) { \
	if (sym.is(symbol::DELIM, symbol::s)) { lexer::getsym(); } \
	else { err << MISSING_##s << std::endl; } \
}
		ASSERT_DELIM(SEMICN)
		ASSERT_DELIM(RPARENT)
		ASSERT_DELIM(RBRACK)
#undef ASSERT_DELIM
	}
}

#endif /* ERROR_H */
