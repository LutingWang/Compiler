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
	friend class StackFrame;
protected:
	std::map<symtable::Entry*, int> _syms;

	int _size = 0;
	
	int _locate(symtable::Entry*) const;
public:
	Sbss(const std::set<symtable::Entry*>& syms);

	bool contains(symtable::Entry*) const;
};

class StackFrame : protected Sbss {
	std::vector<ObjCode>& _output;

	std::map<symtable::Entry*, int> _args;

	int _regBase;

	void _visit(bool isLoad, Reg, symtable::Entry* const = nullptr);
public:
	StackFrame(std::vector<ObjCode>&, 
			std::vector<symtable::Entry*> argList, 
			std::set<symtable::Entry*> syms);

	int size(void) const;

	int operator [] (symtable::Entry* const) const;

	int operator [] (Reg) const;

	void store(Reg);

	void store(Reg, symtable::Entry* const);

	void load(Reg);

	void load(Reg, symtable::Entry* const);
};

#endif /* MEMORY_H */
