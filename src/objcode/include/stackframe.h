/**********************************************
    > File Name: StackFrame.h
    > Author: Luting Wang
    > Mail: 2457348692@qq.com 
    > Created Time: Sun Nov 10 20:50:06 2019
 **********************************************/

#ifndef STACKFRAME_H
#define STACKFRAME_H

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

// class StackFrame {
// public:
// 	class Sbss {
// 		friend class StackFrame;
// 	
// 		std::map<symtable::Entry*, int> _syms;
// 		int _size = 0;
// 	
// 		bool _contains(symtable::Entry*) const;
// 		int operator [] (symtable::Entry*) const;
// 
// 		void _alloc(const std::vector<symtable::Entry*>&, int offset = 0);
// 	public:
// 		Sbss(const std::vector<symtable::Entry*>&);
// 		Sbss(const std::set<symtable::Entry*>&);
// 	};
// 
// private:
// 	std::vector<ObjCode>& _output;
// 	const Sbss& _global;
// 	Sbss _local;
// 
// 	int _regBase; // base offset of regs
// 	int _size;
// 
// 	int _locate(Reg);
// 	void _visit(Reg, symtable::Entry* const, bool isLoad);
// public:
// 	StackFrame(std::vector<ObjCode>&, 
// 			const Sbss& global, 
// 			std::vector<symtable::Entry*> argList, 
// 			const std::set<symtable::Entry*>& syms);
// 
// 	void store(Reg reg, symtable::Entry* const entry) {
// 		_visit(reg, entry, false);
// 	}
// 
// 	void load(Reg reg, symtable::Entry* const entry) {
// 		_visit(reg, entry, true);
// 	}
// };

#endif /* STACKFRAME_H */
