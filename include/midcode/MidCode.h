/**********************************************
    > File Name: MidCode.h
    > Author: Luting Wang
    > Mail: 2457348692@qq.com 
    > Created Time: Mon Nov  4 12:18:57 2019
 **********************************************/

#ifndef MIDCODE_H
#define MIDCODE_H

#include <string>

namespace symtable {
	class Entry;
	class FuncTable;
}

class Optim;

class MidCode {
	friend class Optim;
public:
	enum class Instr {
		ADD,		// t0 = t1 + t2
		SUB,		// t0 = t1 - t2
		MULT,		// t0 = t1 * t2
		DIV,		// t0 = t1 / t2
		LOAD_IND,	// t0 = t1[t2]
		STORE_IND,	// t0[t2] = t1
		ASSIGN,		// t0 = t1

		PUSH_ARG,	// push t1
		CALL,		// t0(nullable) = call t3
		RET,		// return t1(nullable)

		INPUT,		// scanf(t0)
		OUTPUT_STR,	// printf(t3)
		OUTPUT_SYM,	// printf(t1)

		BGT,		// branch to t3 if t1 > t2
		BGE,		// branch to t3 if t1 >= t2
		BLT,		// branch to t3 if t1 < t2
		BLE,		// branch to t3 if t1 <= t2
		BEQ,		// branch to t3 if t1 == t2(nullable)
		BNE,		// branch to t3 if t1 != t2(nullable)
		GOTO,		// goto t3
		LABEL		// lab t3
	};

private:
	MidCode(const Instr, 
			const symtable::Entry* const, 
			const symtable::Entry* const, 
			const symtable::Entry* const, 
			const std::string&);
public:
	~MidCode(void);

public:
	const Instr instr;			// operation
	const symtable::Entry* const t0;	// result variable
	const symtable::Entry* const t1;	// operand1
	const symtable::Entry* const t2;	// operand2
	const std::string t3;		// label name

	bool is(const Instr) const;
	// TODO: make all t's private
	const std::string& labelName(void) const;

	static void gen(const Instr, 
			const symtable::Entry* const, 
			const symtable::Entry* const, 
			const symtable::Entry* const, 
			const std::string& = "");

	// naming convension:
	//     #1		- temporary variable
	//     int$0	- global int constant
	//     char$0	- global int constant
	//     label_1	- non-functional labels
	static const symtable::Entry* genVar(const bool);
	static const symtable::Entry* genConst(const bool, const int);
	static std::string genLabel(void);

private:
	void print(void) const;
	static void print(const symtable::Entry* const); // if `judge` is off, print nothing
	static void print(const symtable::FuncTable* const);
public:
	static void output(void);
};

#endif /* MIDCODE_H */
