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
		BEQ,		// branch to t3 if t1 == t2
		BNE,		// branch to t3 if t1 != t2
		GOTO,		// goto t3
		LABEL		// lab t3
	};

private:
	// All the fields that are stored as pointers must be
    // assigned `nullptr`, including `_t3`. Note that no
    // two `_t3`s should point to the same string, for the
    // sake of avoiding redundent release.
	const Instr _instr;			// operation
	const symtable::Entry* const _t0;	// result variable
	const symtable::Entry* const _t1;	// operand1
	const symtable::Entry* const _t2;	// operand2
	const std::string* _t3;		// label name
public:
    // The fields can only be visited when they are valid (
    // not null), except for the `exceptions` listed in front
    // of each field. The exceptions are instruction classes
    // that can legally contain an invalid field.
	Instr instr(void) const;
	const symtable::Entry* t0(void) const; // exceptions: CALL
	const symtable::Entry* t1(void) const; // exceptions: RET
	const symtable::Entry* t2(void) const;
	const std::string& labelName(void) const;

private:
    // Upon construction, the initializer should check whether
    // these arguments meets with the exact requirements of
    // nullity. The requirements are listed in class `Instr`.
    //
    // Generaly, the initializer is called by `gen`, in order to
    // lower the risk of violating nullity requirements. However
    // `gen` also inserts the midcode into symtable, which is
    // unwanted when optimizing the midcodes. Thus, `Optim` is
    // declared as a friend class and should call this initializer
    // with extra caution.
	MidCode(const Instr, 
			const symtable::Entry* const, 
			const symtable::Entry* const, 
			const symtable::Entry* const, 
			const std::string* const);
    
    // Copy constructor of `MidCode` provided exclusively for
    // `Optim`. Although `_t3` is allocated outside all the
    // initializers, this field is considered under the regulation
    // of `MidCode`. Thus, this initializer should check the nullity
    // of `_t3` and if it is not null, deep clone.
	MidCode(const MidCode&);
public:
    // The only attribute that needs to be released is `_t3`.
    // Remember not to free the `Entry`s since they are still
    // used in other `MidCode`s.
	~MidCode(void);

	// Instructions can be categorized. Knowing which class this
    // midcode belongs to can help user determine which attribute
    // he should visit. The categories are listed below
    //     - calc       ADD / SUB / MULT / DIV
    //     - branch     BGT / BGE / BLT / BLE / BEQ / BNE
    // Other instructions that is not categorized should be judged
    // by user though `is`.
    bool is(const Instr) const;
	bool isCalc(void) const;
	bool isBranch(void) const;

private:
    // If error happened or the current function being parsed has
    // already returned, do nothing. Otherwise push the `MidCode`
    // into symtable.
	static void _gen(const MidCode* const);
public:
	static void gen(const Instr, 
			const symtable::Entry* const, 
			const symtable::Entry* const, 
			const symtable::Entry* const);

	static void gen(const Instr, 
			const symtable::Entry* const, 
			const symtable::Entry* const, 
			const symtable::Entry* const, 
			const std::string&);

	// naming convension:
	//     #1		- temporary variable
	//     int$0	- global int constant
	//     char$0	- global int constant
	//     label_1	- non-functional labels
	static const symtable::Entry* genVar(const bool);
	static const symtable::Entry* genConst(const bool, const int);
	static std::string genLabel(void);

private:
    // print this piece of midcode
	void _print(void) const;
public:
    // print all the midcodes in symtable, grouped by functions
	static void output(void);
};

#endif /* MIDCODE_H */
