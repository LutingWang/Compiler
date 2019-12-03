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

namespace symtable {
	class Entry;
}

class ObjCode;

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
	std::map<const symtable::Entry*, int> _args;
	int _regBase;
	int _size;
public:
	int size(void) const;

	StackFrame(std::vector<const symtable::Entry*> argList,
			const std::set<const symtable::Entry*>& syms);

	int operator [] (const symtable::Entry* const) const;
	int operator [] (Reg) const;

private:
	const ObjCode* _visit(const bool isLoad, const Reg) const;
public:
	const ObjCode* store(Reg) const;
	const ObjCode* load(Reg) const;

private:
	const ObjCode* _visit(const bool isLoad, const Reg, const symtable::Entry* const) const;
public:
	const ObjCode* store(Reg, const symtable::Entry* const) const;
	const ObjCode* load(Reg, const symtable::Entry* const) const;
};

#endif /* MEMORY_H */
