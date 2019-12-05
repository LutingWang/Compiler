/**********************************************
    > File Name: Sbss.cpp
    > Author: Luting Wang
    > Mail: 2457348692@qq.com 
    > Created Time: Mon Nov 11 08:49:47 2019
 **********************************************/

#include <cassert>
#include "symtable/table.h"
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
        if (global() == nullptr) { // `this` is `_global`
            assert(entry->isGlobal());
        } else if (entry->isGlobal()) { // `entry` included in `_global`
            assert(_global->_syms.count(entry));
            continue;
        }
		_syms[entry] = _size;
		if (!entry->isArray()) { _size += WORD_SIZE; }
		else { _size += entry->value() * WORD_SIZE; }
	}
}

int Sbss::locate(const symtable::Entry* entry) const {
	return _syms.at(entry);
}
