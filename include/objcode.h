/**********************************************
    > File Name: objcode.h
    > Author: Luting Wang
    > Mail: 2457348692@qq.com 
    > Created Time: Fri Nov  8 22:43:45 2019
 **********************************************/

#ifndef OBJCODE_H
#define OBJCODE_H

#include <vector>

namespace symtable {
	class Entry;
}

class MidCode;

enum class Reg {
	zero,
	at,
	v0, v1,
	a0, a1, a2, a3,
	t0, t1, t2, t3, t4, t5, t6, t7,
	s0, s1, s2, s3, s4, s5, s6, s7,
	t8, t9,
	gp,
	sp,
	fp,
	ra
};

class RegPool {
	// A record describes the operation upon request of a 
	// temporary register. 
	struct Record {
		Reg reg;
		symtable::Entry* store; // sym to write back
		symtable::Entry* load; // sym to load to reg

		Record(Reg reg, symtable::Entry* store = nullptr, 
				symtable::Entry* load = nullptr) :
			reg(reg), store(store), load(load) {}
	};

	// The first part of `_records` consists of operations
	// need to be done at each request, while the second
	// part stores all the registers back to the stack, so
	// that the basic block can exit safely.
	std::vector<Record> _records;
public:
	// derive `_records` using LRU strategy
	RegPool(std::vector<MidCode*>&);

	// perform one operation and returns the register
	Reg request(void);

	// finish up all the remaining operations
	void clear(void);
};

#endif /* OBJCODE_H */
