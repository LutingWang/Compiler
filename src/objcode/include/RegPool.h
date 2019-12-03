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
    std::vector<const ObjCode*>& _output;
    
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
	// assign s registers
	RegPool(std::vector<const ObjCode*>&,
            const std::vector<const MidCode*>&,
			const std::vector<const symtable::Entry*>& reg_a,
            const StackFrame&);
    
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
