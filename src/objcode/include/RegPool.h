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

class Action;

class RegPool {
	// global registers
	std::vector<const symtable::Entry*> _reg_a;
	std::vector<const symtable::Entry*> _reg_s;

	// cached actions grouped by blocks
	std::vector<std::vector<Action*>> _actionCache;
	int _blockCounter = 0;
	int _actionCounter = 0;

	// execute the current action
	void _execute(StackFrame&);
public:
	// assign global registers
	RegPool(const std::vector<const MidCode*>&, 
			const std::vector<const symtable::Entry*>& reg_a);
    
    ~RegPool(void);
    
	// simulate register assignment
	void simulate(const std::vector<const symtable::Entry*>&, 
			const std::vector<bool>& write, 
			const std::vector<bool>& mask);

	// which syms need to be stored in the stack
	void storage(std::set<const symtable::Entry*>&) const;

	// perform one operation and returns the register
	Reg request(StackFrame&);

	// finish up all the remaining operations in this block
	void clear(StackFrame&);
};

#endif /* REGPOOL_H */
