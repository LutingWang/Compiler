/**********************************************
    > File Name: Sbss.cpp
    > Author: Luting Wang
    > Mail: 2457348692@qq.com 
    > Created Time: Mon Nov 11 08:49:47 2019
 **********************************************/

#include <vector>
#include "symtable.h"

#include "../include/StackFrame.h"

bool Sbss::_contains(symtable::Entry* entry) const {
	return _syms.count(entry);
}

int Sbss::_locate(symtable::Entry* entry) const {
	return _syms.at(entry);
}

Sbss::Sbss(const std::set<symtable::Entry*>& syms) {
	int offset = 0;
	for (auto& entry : syms) {
		assert(!entry->isConst);
		_syms[entry] = offset;
		if (entry->value == -1) { offset += 4; } 
		else { offset += entry->value * 4; }
	}
	_size = offset;
}
