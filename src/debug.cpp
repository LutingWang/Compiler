/**********************************************
    > File Name: output.cpp
    > Author: Luting Wang
    > Mail: 2457348692@qq.com 
    > Created Time: Sun Sep 29 20:41:18 2019
 **********************************************/

#include <iostream>
#include <map>
#include "lexer.h"
#include "debug.h"
using namespace std;

vector<string> v;

string print(Nonterminal name) {
	static map<unsigned int, string> ter = {
		{ PROGRAM, "<程序>" },
		{ CONST_DEC, "<常量说明>" }, { CONST_DEF, "<常量定义>" },
		{ VAR_DEC, "<变量说明>" }, { VAR_DEF, "<变量定义>" },
		{ STR, "<字符串>" },
		{ INT, "<整数>" }, { UNSIGNED_INT, "<无符号整数>" },
		{ FUNC_DEF_WITH_RET, "<有返回值函数定义>" },
		{ FUNC_DEF_WITHOUT_RET, "<无返回值函数定义>" },
		{ FUNC_CALL_WITH_RET, "<有返回值函数调用语句>" },
		{ FUNC_CALL_WITHOUT_RET, "<无返回值函数调用语句>" },
		{ MAIN_FUNC, "<主函数>" },
		{ ARGS, "<参数表>" }, { VALUES, "<值参数表>" },
		{ DEC_HEAD, "<声明头部>" },
		{ BLOCK, "<复合语句>" }, { STAT_SERIES, "<语句列>" },
		{ STAT, "<语句>" }, { ASSIGN, "<赋值语句>" },
		{ READ, "<读语句>" }, { WRITE, "<写语句>" },
		{ RET, "<返回语句>" }, { COND, "<条件语句>" },
		{ ITER, "<循环语句>" }, { CONDITION, "<条件>" },
		{ STEP, "<步长>" },
		{ EXPR, "<表达式>" }, { ITEM, "<项>" },
		{ FACTOR, "<因子>" },
	};
	return ter[name];
}

string print(void) {
	static map<unsigned int, string> res = {
		{ lexer::CONSTTK, "CONSTTK const" }, { lexer::INTTK, "INTTK int" },
		{ lexer::CHARTK, "CHARTK char" }, { lexer::VOIDTK, "VOIDTK void" },
		{ lexer::MAINTK, "MAINTK main" }, { lexer::IFTK, "IFTK if" },
		{ lexer::ELSETK, "ELSETK else" }, { lexer::DOTK, "DOTK do" },
		{ lexer::WHILETK, "WHILETK while" }, { lexer::FORTK, "FORTK for" },
		{ lexer::SCANFTK, "SCANFTK scanf" }, { lexer::PRINTFTK, "PRINTFTK printf" },
		{ lexer::RETURNTK, "RETURNTK return" }
	};
	static map<unsigned int, string> del = {
		{ lexer::ASSIGN, "ASSIGN =" }, { lexer::SEMICN, "SEMICN ;" },
		{ lexer::COMMA, "COMMA ," }, { lexer::LPARENT, "LPARENT (" },
		{ lexer::RPARENT, "RPARENT )" }, { lexer::LBRACK, "LBRACK [" },
		{ lexer::RBRACK, "RBRACK ]" }, { lexer::LBRACE, "LBRACE {" },
		{ lexer::RBRACE, "RBRACE }" }
	};
	static map<unsigned int, string> oper = {
		{ lexer::PLUS, "PLUS +" }, { lexer::MINU, "MINU -" },
		{ lexer::MULT, "MULT *" }, { lexer::DIV, "DIV /" }
	};
	static map<unsigned int, string> comp = {
		{ lexer::LSS, "LSS <" }, { lexer::LEQ, "LEQ <=" },
		{ lexer::GRE, "GRE >" }, { lexer::GEQ, "GEQ >=" },
		{ lexer::EQL, "EQL ==" }, { lexer::NEQ, "NEQ !=" }
	};
	switch (sym.id) {
	case lexer::IDENFR:
		return "IDENFR " + sym.str;
		break;
	case lexer::INTCON:
		return "INTCON " + to_string(sym.num);
		print(UNSIGNED_INT);
		break;
	case lexer::CHARCON:
		return "CHARCON " + string(1, sym.ch);
		break;
	case lexer::STRCON:
		return "STRCON " + sym.str;
		print(STR);
		break;
	case lexer::RESERVED:
		return res[sym.num];
		break;
	case lexer::DELIM:
		return del[sym.num];
		break;
	case lexer::OPER:
		return oper[sym.num];
		break;
	case lexer::COMP:
		return comp[sym.num];
		break;
	default:
		throw "unknown symbol type id";
	}
}
