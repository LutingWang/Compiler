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

#include "./Reg.h"

struct ObjCode;
class StackFrame;

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

class TPool {
    std::vector<const symtable::Entry*> _regs;
    std::vector<bool> _dirty;
    const StackFrame& _stackframe;
public:
    TPool(const StackFrame&);
    
    Reg request(const symtable::Entry* const, const bool write, const Reg mask, const std::vector<const symtable::Entry*>&);
    void writeback(void);
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
    TPool _reg_t;
	const SPool _reg_s;
    const StackFrame& _stackframe;
    
	std::vector<const symtable::Entry*> _seq; // within one block
    Reg _maskCache;

public:
	RegPool(const symtable::FuncTable* const, const StackFrame&);
    
    void genPrologue(void) const;
    void genEpilogue(void) const;
    void stash(void) const;
    void unstash(void) const;
    
	// foresee register usage sequence
	void foresee(const symtable::Entry* const);

	Reg request(const bool write, const bool mask);
	void clear(void);
};

#endif /* REGPOOL_H */
