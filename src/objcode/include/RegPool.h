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
#include "midcode.h"
#include "symtable.h"

#include "Reg.h"

class ObjCode;
class StackFrame;

class Action;

class RegPool {
	std::vector<const symtable::Entry*> _reg_a;
	std::vector<const symtable::Entry*> _reg_s;
    const StackFrame& _stackframe;
	std::queue<Action*> _actionCache; // within one block

public:
	RegPool(const std::vector<const symtable::Entry*>& reg_a,
            const StackFrame&);
    
    void assignSavedRegs(const symtable::FuncTable* const);
    void genPrologue(void) const; // backup s regs and load s regs for this func
    void genEpilogue(void) const; // restore s regs
    
	// simulate register assignment
	void simulate(const std::vector<const symtable::Entry*>&, 
			const std::vector<bool>& write, 
			const std::vector<bool>& mask);

private:
    // execute the current action
    void _execute(void);
public:
	// perform one operation and return the register
	Reg request(void);

	// finish up all the remaining operations in this block
	void clear(void);
};

#endif /* REGPOOL_H */
