/**********************************************
    > File Name: RegPool.h
    > Author: Luting Wang
    > Mail: 2457348692@qq.com 
    > Created Time: Sat Nov  9 22:56:34 2019
 **********************************************/

#ifndef REGPOOL_H
#define REGPOOL_H

#include <queue>
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

	// cached actions within one block
	std::queue<Action*> _actionCache;

	// execute the current action
	void _execute(StackFrame&);
public:
	// assign s registers
	RegPool(const std::vector<const MidCode*>&, 
			const std::vector<const symtable::Entry*>& reg_a);
    
	// simulate register assignment
	void simulate(const std::vector<const symtable::Entry*>&, 
			const std::vector<bool>& write, 
			const std::vector<bool>& mask);

	// perform one operation and returns the register
	Reg request(StackFrame&);

	// finish up all the remaining operations in this block
	void clear(StackFrame&);
};

#endif /* REGPOOL_H */
