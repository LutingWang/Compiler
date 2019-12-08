/**********************************************
    > File Name: RegPool.h
    > Author: Luting Wang
    > Mail: 2457348692@qq.com 
    > Created Time: Sat Nov  9 22:56:34 2019
 **********************************************/

#ifndef REGPOOL_H
#define REGPOOL_H

#include <functional>
#include <map>
#include <queue>
#include <set>
#include <vector>
#include "midcode.h"
#include "symtable.h"

#include "Reg.h"

class ObjCode;
class StackFrame;

class Action;

class SPool {
    std::map<const symtable::Entry*, Reg> _regs;
public:
    SPool(const symtable::FuncTable* const);
    
    bool contains(const symtable::Entry* const) const;
    Reg at(const symtable::Entry* const) const;
    
private:
    void _usage(std::set<Reg>&) const;
public:
    void backup(const StackFrame&) const;
    void restore(const StackFrame&) const;
};

class RegPool {
	std::vector<const symtable::Entry*> _reg_a;
	const SPool _reg_s;
    const StackFrame& _stackframe;
	std::queue<Action*> _actionCache; // within one block

public:
	RegPool(const std::vector<const symtable::Entry*>& reg_a,
            const symtable::FuncTable* const,
            const StackFrame&);
    
    void genPrologue(void) const; // backup s regs
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
