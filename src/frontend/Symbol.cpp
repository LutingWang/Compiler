/**********************************************
    > File Name: Symbol.cpp
    > Author: Luting Wang
    > Mail: 2457348692@qq.com 
    > Created Time: Sat Oct  5 13:25:23 2019
 **********************************************/

#include <typeinfo>
#include "compilerConfig.h"

#include "./Symbol.h"

symbol::Symbol sym;

int symbol::Symbol::lastLine(void) const {
    return _lastLine;
}

symbol::Type symbol::Symbol::id(void) const {
    return _id;
}

char symbol::Symbol::ch(void) const {
    return _ch;
}

unsigned int symbol::Symbol::num(void) const {
    return _num;
}

const std::string& symbol::Symbol::str(void) const {
    return _str;
}

bool symbol::Symbol::is(const Type type) const noexcept {
    return this->_id == type;
}

bool symbol::Symbol::is(const Type type, const unsigned int num) const noexcept {
    return numIs(num) && is(type);
}

bool symbol::Symbol::numIs(const unsigned int num) const noexcept {
    return this->_num & num;
}

void symbol::Symbol::set(const Type type, const unsigned int num) noexcept {
    this->_id = type;
    this->_num = num;
}

#if !judge
std::ostream& symbol::operator << (std::ostream& output, const Symbol& s) {
	output << typeid(s).name() << '<';
	switch (sym.id()) {
#define CASE(id, info) case id: output << #id << ' ' << info; break
	CASE(Type::IDENFR, sym.str()); CASE(Type::INTCON, sym.num());
	CASE(Type::CHARCON, sym.ch()); CASE(Type::STRCON, sym.str());
	case Type::RESERVED:
		switch (sym.num()) {
		CASE(CONSTTK, "const"); CASE(INTTK, "int"); CASE(CHARTK, "char"); 
		CASE(VOIDTK, "void"); CASE(MAINTK, "main"); CASE(IFTK, "if");
		CASE(ELSETK, "else"); CASE(DOTK, "do"); CASE(WHILETK, "while"); 
		CASE(FORTK, "for"); CASE(SCANFTK, "scanf"); CASE(PRINTFTK, "printf");
		CASE(RETURNTK, "return");
		}
		break;
	case Type::DELIM:
		switch (sym.num()) {
		CASE(ASSIGN, "="); CASE(SEMICN, ";"); CASE(COMMA, ","); 
		CASE(LPARENT, "("); CASE(RPARENT, ")"); CASE(LBRACK, "["); 
		CASE(RBRACK, "]"); CASE(LBRACE, "{"); CASE(RBRACE, "}");
		}
		break;
	case Type::OPER:
		switch (sym.num()) {
		CASE(PLUS, "+"); CASE(MINU, "-"); CASE(MULT, "*"); CASE(DIV, "/");
		}
		break;
	case Type::COMP:
		switch (sym.num()) {
		CASE(LSS, "<"); CASE(LEQ, "<="); CASE(GRE, ">"); 
		CASE(GEQ, ">="); CASE(EQL, "=="); CASE(NEQ, "!=");
		}
		break;
	default: throw "unknown symbol type id";
#undef CASE
	}
	output << '>';
	return output;
}

std::ostream& symbol::operator << (std::ostream& output, const NonTerminal& nt) {
	output << typeid(nt).name() << '<';
	switch (nt) {
#define CASE(id, info) case NonTerminal::id: output << #info; break
	CASE(LEX_UNSIGNED_INT, 无符号整数); CASE(LEX_STR, 字符串); CASE(PROGRAM, 程序);
	CASE(CONST_DEC, 常量说明); CASE(CONST_DEF, 常量定义); CASE(VAR_DEC, 变量说明); 
	CASE(VAR_DEF, 变量定义);
	CASE(FUNC_DEF_WITH_RET, 有返回值函数定义); CASE(FUNC_DEF_WITHOUT_RET, 无返回值函数定义);
	CASE(FUNC_CALL_WITH_RET, 有返回值函数调用语句); CASE(FUNC_CALL_WITHOUT_RET, 无返回值函数调用语句);
	CASE(FUNC_ARGS_DEC, 参数表); CASE(FUNC_ARGS_VALUE, 值参数表); CASE(FUNC_MAIN, 主函数); 
	CASE(FUNC_DEC_HEAD, 声明头部);
	CASE(STAT_BLOCK, 复合语句); CASE(STAT_SERIES, 语句列); CASE(STAT, 语句); CASE(STAT_INT, 整数);
	CASE(STAT_ASSIGN, 赋值语句); CASE(STAT_READ, 读语句); CASE(STAT_WRITE, 写语句); 
	CASE(STAT_RET, 返回语句);
	CASE(STAT_COND_COND, 条件语句); CASE(STAT_COND_ITER, 循环语句); CASE(STAT_COND, 条件); 
	CASE(STAT_COND_ITER_STEP, 步长);
	CASE(EXPR, 表达式); CASE(EXPR_ITEM, 项); CASE(EXPR_FACTOR, 因子);
#undef CASE
	}
	output << '>';
	return output;
}
#endif /* judge */
