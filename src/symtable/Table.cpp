/**********************************************
    > File Name: Table.cpp
    > Author: Luting Wang
    > Mail: 2457348692@qq.com 
    > Created Time: Tue Oct  1 12:10:59 2019
 **********************************************/

#include <fstream>
#include <typeinfo>
#include "error.h"
#include "MidCode.h"

#include "Printer.h"

#include "symtable.h"
using namespace std;

symtable::Table::~Table(void) {
	for (auto& e : _syms) { delete e.second; }
}

symtable::Entry* symtable::Table::push(const string& symName, const bool isConst, const bool isInt, const int value) {
	Entry*& entry = _syms[symName];
	if (entry != nullptr) { error::raise(error::Code::REDEF); } 
	else {
		entry = new Entry(name() + '_' + symName, isConst, isInt, value);
		Printer::print(*entry);
	}
	return entry;
}

symtable::FuncTable::~FuncTable(void) {
	for (auto& mc : _midcode) { delete mc; }
}
