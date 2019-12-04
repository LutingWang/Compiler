/**********************************************
    > File Name: memory.h
    > Author: Luting Wang
    > Mail: 2457348692@qq.com 
    > Created Time: Wed Nov 13 21:16:19 2019
 **********************************************/

#ifndef MEMORY_H
#define MEMORY_H

#include <map>
#include <set>
#include <vector>

#include "Reg.h"
#include "ObjCode.h"

namespace symtable {
	class Entry;
}

class Sbss {
	static const Sbss* _global;
protected:
	static const Sbss* global(void);
public:
	static void init(void);
	static void deinit(void);

private:
	std::map<const symtable::Entry*, int> _syms;
	int _size = 0;
protected:
	int size(void) const;
	
public:
	Sbss(const std::set<const symtable::Entry*>& syms);
    virtual ~Sbss(void) {}

	virtual int locate(const symtable::Entry*) const;
};

class StackFrame : protected Sbss {
    CodeGen& _output;
	std::map<const symtable::Entry*, int> _args;
	int _regBase;
	int _size;
public:
	int size(void) const;

	StackFrame(CodeGen&, std::vector<const symtable::Entry*> argList,
			const std::set<const symtable::Entry*>& syms);
    virtual ~StackFrame(void) {}

	virtual int locate(const symtable::Entry* const) const;
	int locate(const Reg) const;

private:
    // Load (store) the `reg` from (to) stackframe
	void _visit(const ObjCode::Instr, const Reg) const;
    
    // Load (store) the `entry` from (to) stackframe or sbss. For constants,
    // `instr` would be `li`.
    void _visit(ObjCode::Instr, const Reg, const symtable::Entry* const) const;
    
    // Load (store) the `ind`th element in array `entry` from (to) stackframe
    // or sbss. Note that ind has already been left-shifted.
    void _visit(const ObjCode::Instr, const Reg dst, const symtable::Entry* const, const Reg ind) const;
public:
	void storeReg(const Reg) const;
	void loadReg(const Reg) const;
	void storeSym(const Reg, const symtable::Entry* const) const;
	void loadSym(const Reg, const symtable::Entry* const) const;
    void storeInd(const Reg dst, const symtable::Entry* const, const Reg ind) const;
    void loadInd(const Reg dst, const symtable::Entry* const, const Reg ind) const;
};

#endif /* MEMORY_H */
