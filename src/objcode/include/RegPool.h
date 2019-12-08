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

class APool {
    std::vector<const symtable::Entry*> _regs;
    const StackFrame& _stackframe;
public:
    APool(const symtable::FuncTable* const, const StackFrame&);
    
    bool contains(const symtable::Entry* const) const;
    Reg at(const symtable::Entry* const) const;
    
    void backup(void) const;
    void restore(void) const;
};

class SPool {
    std::map<const symtable::Entry*, Reg> _regs;
    const StackFrame& _stackframe;
public:
    SPool(const symtable::FuncTable* const, const StackFrame&);
    
    bool contains(const symtable::Entry* const) const;
    Reg at(const symtable::Entry* const) const;
    
private:
    void _usage(std::set<Reg>&) const;
public:
    void backup(void) const;
    void restore(void) const;
};

class RegPool {
	const APool _reg_a;
	const SPool _reg_s;
    const StackFrame& _stackframe;
	std::queue<Action*> _actionCache; // within one block

public:
	RegPool(const symtable::FuncTable* const, const StackFrame&);
    
    void genPrologue(void) const;
    void genEpilogue(void) const;
    void stash(void) const;
    void unstash(void) const;
    
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
