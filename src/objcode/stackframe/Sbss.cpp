/**********************************************
    > File Name: Sbss.cpp
    > Author: Luting Wang
    > Mail: 2457348692@qq.com 
    > Created Time: Mon Nov 11 08:49:47 2019
 **********************************************/

#include <cassert>
#include <set>
#include <vector>
#include "symtable.h"

#include "../include/memory.h"

const Sbss* Sbss::_global = nullptr;

const Sbss* Sbss::global(void) {
	return _global;
}

void Sbss::init(void) {
	assert(_global == nullptr);
	std::set<symtable::Entry*> globalSyms;
	table.global().syms(globalSyms);
	_global = new Sbss(globalSyms);
}

void Sbss::deinit(void) {
	delete _global;
	_global = nullptr;
}

int Sbss::size(void) const {
	return _size;
}

Sbss::Sbss(const std::set<symtable::Entry*>& syms) {
	for (auto& entry : syms) {
		if (entry->isConst) { continue; }
		if (global() != nullptr && // this may be the global
				global()->contains(entry)) { continue; }
		_syms[entry] = _size;
		if (entry->value == -1) { _size += 4; } 
		else { _size += entry->value * 4; }
	}
}

bool Sbss::contains(symtable::Entry* entry) const {
	return _syms.count(entry);
}

int Sbss::locate(symtable::Entry* entry) const {
	return _syms.at(entry);
}
