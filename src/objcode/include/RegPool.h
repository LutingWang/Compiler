/**********************************************
    > File Name: RegPool.h
    > Author: Luting Wang
    > Mail: 2457348692@qq.com 
    > Created Time: Sat Nov  9 22:56:34 2019
 **********************************************/

#ifndef REGPOOL_H
#define REGPOOL_H

#include <functional>
#include <queue>
#include <set>
#include <vector>
#include "symtable/table.h"

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
    
    const StackFrame& _stackframe;

	// cached actions within one block
	std::queue<Action*> _actionCache;
    
public:
    const StackFrame& stackframe(void) const;

private:
	// execute the current action
	void _execute(void);
public:
	RegPool(const std::vector<const symtable::Entry*>& reg_a,
            const StackFrame&);
    
    void assignSavedRegs(const symtable::FuncTable* const);
    
	// simulate register assignment
	void simulate(const std::vector<const symtable::Entry*>&, 
			const std::vector<bool>& write, 
			const std::vector<bool>& mask);

	// perform one operation and returns the register
	Reg request(void);

	// finish up all the remaining operations in this block
	void clear(void);
};

#endif /* REGPOOL_H */
