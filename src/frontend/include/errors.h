/**********************************************
    > File Name: errors.h
    > Author: Luting Wang
    > Mail: 2457348692@qq.com 
    > Created Time: Wed Oct  2 21:29:35 2019
 **********************************************/

#ifdef ERROR_H
	#error global error.h cannot exist with local
#endif /* ERROR_H*/

#ifndef ERRORS_H
#define ERRORS_H

#include <exception>

#include "./Symbol.h"

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

	void assertSymIsSEMICN(void);
	void assertSymIsRPARENT(void);
	void assertSymIsRBRACK(void);
}

#endif /* ERRORS_H */
