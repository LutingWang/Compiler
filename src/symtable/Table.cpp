/**********************************************
    > File Name: Table.cpp
    > Author: Luting Wang
    > Mail: 2457348692@qq.com 
    > Created Time: Tue Oct  1 12:10:59 2019
 **********************************************/

#include <cassert>
#include <fstream>
#include <typeinfo>
#include "error.h"
#include "midcode.h"

#include "Printer.h"

#include "symtable.h"
using namespace std;

symtable::Table::~Table(void) {
	for (auto& entry : _syms) { delete entry.second; }
}

bool symtable::Table::contains(const std::string& symName) const {
    return _syms.count(symName);
}

symtable::Entry* symtable::Table::find(const std::string& symName) const {
    return _syms.at(symName);
}

symtable::Entry* symtable::Table::push(const string& symName, const bool isConst, const bool isInt, const int value) {
	if (contains(symName)) {
        error::raise(error::Code::REDEF);
    } else {
        _syms[symName] = new Entry(name() + '_' + symName, isConst, isInt, value);
		Printer::print(*find(symName));
	}
	return find(symName);
}

symtable::FuncTable::~FuncTable(void) {
	for (auto& midcode : midcodes()) { delete midcode; }
}

const std::vector<MidCode*>& symtable::FuncTable::midcodes(void) const {
	return _midcode;
}

void symtable::Table::syms(std::set<Entry*>& symList) const {
	assert(symList.empty());
	for (auto& pair : _syms) {
		auto result = symList.insert(pair.second);
		assert(result.second);
	}
}
