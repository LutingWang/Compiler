/**********************************************
    > File Name: MidCode.h
    > Author: Luting Wang
    > Mail: 2457348692@qq.com 
    > Created Time: Mon Nov  4 12:18:57 2019
 **********************************************/

#ifndef MIDCODE_H
#define MIDCODE_H

#include <iostream>
#include <string>

namespace symtable {
	class Entry;
	class Database;
}

class MidCode {
	friend class symtable::Database;
	friend std::ostream& operator << (std::ostream&, MidCode&);
public:
	enum class Instr {
		ADD,		// t0 = t1 + t2
		SUB,		// t0 = t1(nullable) - t2
		MULT,		// t0 = t1 * t2
		DIV,		// t0 = t1 / t2
		LOAD_IND,	// t0 = t1[t2]
		STORE_IND,	// t0[t2] = t1
		ASSIGN,		// t0 = t1

		PUSH_ARG,	// push t1
		CALL,		// t0(nullable) = call t3
		RET,		// return t1(nullable)

		INPUT,		// scanf(t0)
		OUTPUT,		// printf(t3(nullable), t1(nullable)) cannot have both entry being null

		BGT,		// branch to t3 if t1 > t2
		BGE,		// branch to t3 if t1 >= t2
		BLT,		// branch to t3 if t1 < t2
		BLE,		// branch to t3 if t1 <= t2
		BEQ,		// branch to t3 if t1 == t2(nullable)
		BNE,		// branch to t3 if t1 != t2(nullable)
		GOTO,		// goto t3
		LABEL,		// lab t3
	};

private:
	MidCode(const Instr, symtable::Entry* const, symtable::Entry* const, 
			symtable::Entry* const, const std::string&);

public:
	const Instr instr;			// operation
	symtable::Entry* const t0;	// result variable
	symtable::Entry* const t1;	// operand1
	symtable::Entry* const t2;	// operand2
	const std::string t3;		// label name

	static void gen(const Instr, symtable::Entry* const, symtable::Entry* const, 
			symtable::Entry* const, const std::string& = "");
	static symtable::Entry* genVar(const bool);
	static symtable::Entry* genConst(const bool, const int);
	static std::string genLabel(void);

	static void print(std::ostream&);
};

#endif /* MIDCODE_H */
