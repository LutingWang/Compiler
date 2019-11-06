/**********************************************
    > File Name: error.h
    > Author: Luting Wang
    > Mail: 2457348692@qq.com 
    > Created Time: Wed Oct  2 21:29:35 2019
 **********************************************/

#ifndef ERROR_H
#define ERROR_H

#include <exception>
#include "Symbol.h"

namespace lexer {
	void getsym(void);
}

namespace error {
	extern bool happened;

	enum class Code {
		LEX, REDEF, NODEF,
		MISMATCHED_ARG_NUM, MISMATCHED_ARG_TYPE, MISMATCHED_COND_TYPE,
		ILLEGAL_RET_WITH_VAL, ILLEGAL_RET_WITHOUT_VAL, ILLEGAL_IND, ILLEGAL_ASSIGN, 
		MISSING_SEMICN, MISSING_RPARENT, MISSING_RBRACK, MISSING_WHILE, 
		EXPECTED_LITERAL, UNEXPECTED_EOF
	};

	void raise(Code);

	class Ueof : public std::exception {};

	namespace {
#ifdef ASSERT_DELIM
	#error macro conflict
#endif
#define ASSERT_DELIM(s) inline void assertSymIs##s(void) {				\
	if (sym.is(symbol::Type::DELIM, symbol::s)) { lexer::getsym(); }	\
	else { raise(Code::MISSING_##s); }									\
}
		ASSERT_DELIM(SEMICN)
		ASSERT_DELIM(RPARENT)
		ASSERT_DELIM(RBRACK)
#undef ASSERT_DELIM
	}
}

#endif /* ERROR_H */
