/**********************************************
    > File Name: StackFrame.cpp
    > Author: Luting Wang
    > Mail: 2457348692@qq.com 
    > Created Time: Sun Nov 10 22:12:08 2019
 **********************************************/

#include <algorithm>
#include <map>
#include <set>
#include <vector>
#include "symtable.h"

#include "../include/Reg.h"

#include "../include/stackframe.h"

StackFrame::StackFrame(std::vector<ObjCode>& output, 
		const Sbss& global, 
		std::vector<symtable::Entry*> argList, 
		const std::set<symtable::Entry*>& syms) :
	_output(output), _global(global), _local(syms) {

	for (auto& entry : syms) {
		assert(std::find(argList.begin(), argList.end(), entry) == argList.end());
		assert(!global._contains(entry));
	}

	_regBase = _local._size;
	int regSize = (reg::s.size() + reg::t.size() + 1 /* ra */) * 4;

	while (argList.size() < 4) { argList.push_back(nullptr); }
	for (int i = 0; i < 4; i++) { argList[i] = nullptr; }
	_local._alloc(argList, _regBase + regSize);

	_size = regSize + _local._size;
}

int StackFrame::_locate(Reg reg) {
	static const std::vector<Reg> regs = {
		Reg::s0, Reg::s1, Reg::s2, Reg::s3, Reg::s4, Reg::s5, Reg::s6, Reg::s7,
		Reg::t0, Reg::t1, Reg::t2, Reg::t3, Reg::t4, Reg::t5, Reg::t6, Reg::t7,
		Reg::ra,
		Reg::a0, Reg::a1, Reg::a2, Reg::a3
	};

	int offset = std::find(regs.begin(), regs.end(), reg) - regs.begin();
	assert(offset < regs.size());
	offset = _regBase + offset * 4;
	assert(offset < _size);
	return offset;
}

// TODO: finish
void StackFrame::_visit(Reg reg, symtable::Entry* const entry, bool isLoad) {
	assert(!entry->isConst);
	if (_local._contains(entry)) {

	} else {

	}
}
