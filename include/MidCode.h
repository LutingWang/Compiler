/**********************************************
    > File Name: midcode.h
    > Author: Luting Wang
    > Mail: 2457348692@qq.com 
    > Created Time: Mon Nov  4 12:18:57 2019
 **********************************************/

#ifndef MIDCODE_H
#define MIDCODE_H

#include <iostream>
#include <set>
#include <string>
#include <vector>

// TODO: split into multiple files

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

	// naming convension:
	//     #1		- temporary variable
	//     int$0	- global int constant
	//     char$0	- global int constant
	//     label#1	- non-functional labels
	static symtable::Entry* genVar(const bool);
	static symtable::Entry* genConst(const bool, const int);
	static std::string genLabel(void);

private:
	void print(void) const;
	static void print(symtable::Entry* const); // if `judge` is off, print nothing
	static void print(const symtable::FuncTable* const);
public:
	// The output is only valid before optimization. 
	static void output(void);
};

class BasicBlock {
	friend class FlowChart;

	BasicBlock(const std::vector<MidCode*>&);

	// Add a basic block to `_successors` and
	// add `this` to `_precursors` of the basic
	// block.
	void proceed(BasicBlock* const);
public:
	std::vector<MidCode*> midcodes;
	std::set<BasicBlock*> prec; // precursors
	std::set<BasicBlock*> succ; // successors

	bool isFuncCall(void) const;
};

class FlowChart {
	BasicBlock* _tail;
public:
	std::vector<BasicBlock*> blocks;

	FlowChart(const std::vector<MidCode*>&);

	// deallocate `blocks`
	~FlowChart(void);

	// merge all the mid codes in `blocks` into 
	// one vector and return
	std::vector<MidCode*> output(void) const;
};

#endif /* MIDCODE_H */
