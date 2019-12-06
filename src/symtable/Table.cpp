/**********************************************
    > File Name: Table.cpp
    > Author: Luting Wang
    > Mail: 2457348692@qq.com 
    > Created Time: Tue Oct  1 12:10:59 2019
 **********************************************/

#include <cassert>
#include "compilerConfig.h"
#include "error.h"
#include "symtable/Entry.h"

#include "Printer.h"

#include "symtable/table.h"
using namespace symtable;

void Table::_makeConst(void) {
	assert(!_const);
	_const = true;
}

const std::string& Table::name(void) const { 
	return _name; 
}

void Table::syms(std::set<const Entry*>& symList) const {
	assert(_const);
	assert(symList.empty());
	for (auto& /* <renamed_symName: string, const Entry*> */ pair : _syms) {
		auto result = symList.insert(pair.second);
		assert(result.second);
	}
    assert(symList.count(nullptr) == 0);
}

bool Table::isGlobal(void) const {
    return true;
}

Table::Table(const std::string& name) : 
	_name(name) {}

Table::~Table(void) {
	assert(_const);
}

std::string Table::_rename(const std::string& symName) const {
	return name() + '_' + symName;
}

bool Table::_contains(const std::string& symName) const {
    return _syms.count(symName);
}

const Entry* Table::_find(const std::string& symName) const {
    return _syms.at(symName);
}

bool Table::contains(const std::string& symName) const {
	return _contains(_rename(symName));
}

const Entry* Table::find(const std::string& symName) const {
	return _find(_rename(symName));
}

const Entry* Table::_push(const Entry* const entry) {
	if (_contains(entry->name())) {
        error::raise(error::Code::REDEF);
		delete _find(entry->name());
	}
	_syms[entry->name()] = entry;
#if !judge
    Printer::print(*entry);
#endif /* judge */
    return entry;
}

const Entry* Table::_pushInvalid(const std::string& symName) {
    error::raise(error::Code::NODEF);
    return _push(new Entry(_rename(symName)));
}

const Entry* Table::pushConst(const std::string& symName, 
		const bool isInt, const int value) {
	return _push(new Entry(_rename(symName), isGlobal(), true, isInt, value));
}

const Entry* Table::pushArray(const std::string& symName, 
		const bool isInt, const int value) {
	return _push(new Entry(_rename(symName), isGlobal(), false, isInt, value));
}

const Entry* Table::pushVar(const std::string& symName, const bool isInt) {
	return _push(new Entry(_rename(symName), isGlobal(), isInt));
}

