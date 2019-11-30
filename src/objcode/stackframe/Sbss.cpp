/**********************************************
    > File Name: Sbss.cpp
    > Author: Luting Wang
    > Mail: 2457348692@qq.com 
    > Created Time: Mon Nov 11 08:49:47 2019
 **********************************************/

#include <cassert>
#include <set>
#include <vector>
#include "symtable/table.h"
#include "symtable/Entry.h"
#include "symtable/SymTable.h"

#include "../include/memory.h"

const Sbss* Sbss::_global = nullptr;

const Sbss* Sbss::global(void) {
	return _global;
}

void Sbss::init(void) {
	assert(_global == nullptr);
	std::set<const symtable::Entry*> globalSyms;
	SymTable::getTable().global().syms(globalSyms);
	_global = new Sbss(globalSyms);
}

void Sbss::deinit(void) {
	delete _global;
	_global = nullptr;
}

int Sbss::size(void) const {
	return _size;
}

Sbss::Sbss(const std::set<const symtable::Entry*>& syms) {
	for (auto& entry : syms) {
		if (entry->isConst()) { continue; }
        if (global() == nullptr) { // `this` is the global
            assert(entry->isGlobal());
        } else if (global()->contains(entry)) {
            assert(entry->isGlobal());
            continue;
        } else { assert(!entry->isGlobal()); }
		_syms[entry] = _size;
		if (!entry->isArray()) { _size += 4; } 
		else { _size += entry->value() * 4; }
	}
}

bool Sbss::contains(const symtable::Entry* entry) const {
	return _syms.count(entry);
}

int Sbss::locate(const symtable::Entry* entry) const {
	return _syms.at(entry);
}
