/**********************************************
    > File Name: error.cpp
    > Author: Luting Wang
    > Mail: 2457348692@qq.com 
    > Created Time: Fri Oct 18 23:15:00 2019
 **********************************************/

#include "InputFile.h"
#include "error.h"

std::ostream& error::operator << (std::ostream& output, Code c) {
	// missing errors need to output line number of the last symbol
	switch (c) {
	case Code::LEX:
		output << input.line() << ": lexical error";
		break;
	case Code::REDEF:
		output << input.line() << ": redefinition";
		break;
	case Code::NODEF:
		output << input.line() << ": use of undeclared identifier";
		break;
	case Code::MISMATCHED_ARG_NUM:
		output << input.line() << ": mismatched argument number";
		break;
	case Code::MISMATCHED_ARG_TYPE: 
		output << input.line() << ": mismatched argument type";
		break;
	case Code::MISMATCHED_COND_TYPE:
		output << input.line() << ": illegal condition type";
		break;
	case Code::ILLEGAL_RET_WITH_VAL: 
		output << input.line() << ": return statement with value is not allowed in void functions";
		break;
	case Code::ILLEGAL_RET_WITHOUT_VAL: 
		output << input.line() << ": expected return statment with value";
		break;
	case Code::ILLEGAL_IND: 
		output << input.line() << ": illegal index type";
		break;
	case Code::ILLEGAL_ASSIGN: 
		output << input.line() << ": illegal assignment";
		break;
	case Code::MISSING_SEMICN: 
		output << sym.lastLine << ": missing semicolon";
		break;
	case Code::MISSING_RPARENT:
		output << sym.lastLine << ": missing right parent";
		break;
	case Code::MISSING_RBRACK:
		output << sym.lastLine << ": missing right brack";
		break;
	case Code::MISSING_WHILE:
		output << sym.lastLine << ": missing while in do-while statment";
		break;
	case Code::EXPECTED_LITERAL:
		output << input.line() << ": expected literal";
		break;
	case Code::UNEXPECTED_EOF:
		output << input.line() << ": file ended unexpectedly";
		break;
	default:
		output << input.line() << ": unknown error";
	}
	return output;
}
