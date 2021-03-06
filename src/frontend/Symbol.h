/**********************************************
    > File Name: Symbol.h
    > Author: Luting Wang
    > Mail: 2457348692@qq.com 
    > Created Time: Thu Oct  3 21:03:37 2019
 **********************************************/

#ifndef SYMBOL_H
#define SYMBOL_H

#include <iostream>
#include <string>

class Lexer;

namespace symbol {
	enum class NonTerminal {
		LEX_UNSIGNED_INT, LEX_STR, PROGRAM, CONST_DEC, CONST_DEF, VAR_DEC, VAR_DEF,
		FUNC_DEF_WITH_RET, FUNC_DEF_WITHOUT_RET, FUNC_CALL_WITH_RET, FUNC_CALL_WITHOUT_RET, 
		FUNC_ARGS_DEC, FUNC_ARGS_VALUE, FUNC_MAIN, FUNC_DEC_HEAD, 
		STAT_BLOCK, STAT_SERIES, STAT, STAT_INT, STAT_ASSIGN, STAT_READ, STAT_WRITE, STAT_RET,
		STAT_COND_COND, STAT_COND_ITER, STAT_COND, STAT_COND_ITER_STEP, 
		EXPR, EXPR_ITEM, EXPR_FACTOR 
	};

	enum class Type { IDENFR, INTCON, CHARCON, STRCON, RESERVED, DELIM, OPER, COMP };

	enum Reserved {
		CONSTTK		= 0x0001, 
		INTTK		= 0x0002, 
		CHARTK		= 0x0004, 
		VOIDTK		= 0x0008, 
		MAINTK		= 0x0010, 
		IFTK		= 0x0020, 
		ELSETK		= 0x0040, 
		DOTK		= 0x0080, 
		WHILETK		= 0x0100, 
		FORTK		= 0x0200, 
		SCANFTK		= 0x0400, 
		PRINTFTK	= 0x0800, 
		RETURNTK	= 0x1000
	};

	enum Delim { 
		ASSIGN	= 0x001, 
		SEMICN	= 0x002, 
		COMMA	= 0x004, 
		LPARENT	= 0x008, 
		RPARENT	= 0x010, 
		LBRACK	= 0x020, 
		RBRACK	= 0x040, 
		LBRACE	= 0x080, 
		RBRACE	= 0x100 
	};

	enum Oper { 
		PLUS	= 0x1, 
		MINU	= 0x2, 
		MULT	= 0x4, 
		DIV		= 0x8 
	};

	enum Comp { 
		LSS	= 0x01, 
		LEQ = 0x02, 
		GRE = 0x04, 
		GEQ = 0x08, 
		EQL = 0x10, 
		NEQ = 0x20 
	};

	class Symbol {
		friend class ::Lexer;
		friend std::ostream& operator << (std::ostream&, const symbol::Symbol&);

		// line number of the previous symbol
		int _lastLine;

		// type identifier for the symbol
		Type _id;

		// union of symbol value
		char _ch;
		unsigned int _num;
		std::string _str;
	public:
		int lastLine(void) const;
		Type id(void) const;
		char ch(void) const;
		unsigned int num(void) const;
		const std::string& str(void) const;

		bool is(const Type) const noexcept;
		bool is(const Type, const unsigned int) const noexcept;
		bool numIs(const unsigned int) const noexcept;

		void set(const Type, const unsigned int) noexcept;
	};

	std::ostream& operator << (std::ostream&, const NonTerminal&);
}

extern symbol::Symbol sym;

#endif /* SYMBOL_H */
