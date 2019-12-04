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

	bool contains(const symtable::Entry*) const;
	int locate(const symtable::Entry*) const;
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

	int operator [] (const symtable::Entry* const) const;
	int operator [] (Reg) const;

private:
	void _visit(const ObjCode::Instr, const Reg) const;
public:
	void store(Reg) const;
	void load(Reg) const;

private:
    // instr may alter for constants
	void _visit(ObjCode::Instr, const Reg, const symtable::Entry* const) const;
public:
	void store(Reg, const symtable::Entry* const) const;
	void load(Reg, const symtable::Entry* const) const;
};

#endif /* MEMORY_H */
