/**********************************************
    > File Name: Sbss.cpp
    > Author: Luting Wang
    > Mail: 2457348692@qq.com 
    > Created Time: Mon Nov 11 08:49:47 2019
 **********************************************/

#include <vector>
#include "symtable.h"

#include "../include/stackframe.h"

bool StackFrame::Sbss::_contains(symtable::Entry* entry) const {
	return _syms.count(entry);
}

int StackFrame::Sbss::operator [] (symtable::Entry* entry) const {
	return _syms.at(entry);
}

void StackFrame::Sbss::_alloc(const std::vector<symtable::Entry*>& syms, int offset) {
	int top = offset;
	for (auto& entry : syms) {
		assert(!entry->isConst);
		_syms[entry] = top;
		if (entry->value == -1) { top += 4; }
		else { top += entry->value * 4; }
	}

	_size += top - offset;
}

StackFrame::Sbss::Sbss(const std::vector<symtable::Entry*>& syms) {
	_alloc(syms);
}

StackFrame::Sbss::Sbss(const std::set<symtable::Entry*>& syms) {
	_alloc(std::vector<symtable::Entry*>(syms.begin(), syms.end()));
}
