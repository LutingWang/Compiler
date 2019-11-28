/**********************************************
    > File Name: error.cpp
    > Author: Luting Wang
    > Mail: 2457348692@qq.com 
    > Created Time: Fri Oct 18 23:15:00 2019
 **********************************************/

#include <fstream>
#include "compilerConfig.h"

#include "./include/errors.h"
#include "./include/Lexer.h"
#include "./include/InputFile.h"

extern std::ofstream error_output;

bool error::happened = false;

void error::raise(Code c) {
	error::happened = true;

	switch (c) {
#if judge
	#define CASE(id, msg) case Code::id:	\
		error_output << Lexer::input().line() << ' '	\
			<< (char) ((int) c + 'a')		\
			<< std::endl;					\
		break
#else
	#define CASE(id, msg) case Code::id:	\
		error_output << Lexer::input().line()		\
			<< ": " #msg << std::endl;		\
		break
#endif /* CASE */
	CASE(LEX, lexical error);
	CASE(REDEF, redefinition);
	CASE(NODEF, use of undeclared identifier);
	CASE(MISMATCHED_ARG_NUM, mismatched argument number);
	CASE(MISMATCHED_ARG_TYPE, mismatched argument type);
	CASE(MISMATCHED_COND_TYPE, illegal condition type);
	CASE(ILLEGAL_RET_WITH_VAL, return statement with value is not allowed in void functions);
	CASE(ILLEGAL_RET_WITHOUT_VAL, expected return statment with value);
	CASE(ILLEGAL_IND, illegal index type);
	CASE(ILLEGAL_ASSIGN, illegal assignment);
	CASE(EXPECTED_LITERAL, expected literal);
	CASE(UNEXPECTED_EOF, file ended unexpectedly);
#undef CASE

#if judge
	#define CASE(id, msg) case Code::id:	\
		error_output << sym.lastLine() << ' '	\
			<< (char) ((int) c + 'a')		\
			<< std::endl;					\
		break
#else
	#define CASE(id, msg) case Code::id:	\
		error_output << sym.lastLine()		\
			<< ": " #msg << std::endl;		\
		break
#endif /* CASE */
	// missing errors need to output line number of the last symbol
	CASE(MISSING_SEMICN, missing semicolon);
	CASE(MISSING_RPARENT, missing right parent);
	CASE(MISSING_RBRACK, missing right brack);
	CASE(MISSING_WHILE, missing while in do-while statment);
#undef CASE

    default: error_output << Lexer::input().line() << ": unknown error" << std::endl;
	}
}

#define ASSERT_DELIM(s) void error::assertSymIs##s(void) {				\
	if (sym.is(symbol::Type::DELIM, symbol::s)) { Lexer::getsym(); }	\
	else { raise(Code::MISSING_##s); }									\
}
	ASSERT_DELIM(SEMICN)
	ASSERT_DELIM(RPARENT)
	ASSERT_DELIM(RBRACK)
#undef ASSERT_DELIM
