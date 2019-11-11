/**********************************************
    > File Name: RegPool.h
    > Author: Luting Wang
    > Mail: 2457348692@qq.com 
    > Created Time: Sat Nov  9 22:56:34 2019
 **********************************************/

#ifndef REGPOOL_H
#define REGPOOL_H

#include <set>
#include <vector>

#include "Reg.h"

namespace symtable {
	class Entry;
}

class MidCode;

class ObjCode;
class StackFrame;

struct Action;

class RegPool {
	// global registers
	std::vector<symtable::Entry*> _reg_a;
	std::vector<symtable::Entry*> _reg_s;

	void assignRegS(const std::vector<MidCode*>&);

	// actions grouped by blocks
	std::vector<std::vector<Action>> _actions;
	int _blockCounter = 0;
	int _actionCounter = 0;

	void execute(const StackFrame&);

public:
	// derive `_actions` using simulator
	RegPool(const std::vector<MidCode*>&, 
			const std::vector<symtable::Entry*>& reg_a);

	// which syms need to be stored in the stack
	void storage(std::set<symtable::Entry*>&) const;

	// perform one operation and returns the register
	Reg request(const StackFrame&);

	// finish up all the remaining operations in this block
	void clear(const StackFrame&);
};

#endif /* REGPOOL_H */
