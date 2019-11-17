/**********************************************
    > File Name: StrPool.cpp
    > Author: Luting Wang
    > Mail: 2457348692@qq.com 
    > Created Time: Wed Nov 13 20:04:58 2019
 **********************************************/

#include <fstream>
#include <map>
#include <string>
#include <vector>
#include "symtable/table.h"
#include "symtable/SymTable.h"
#include "midcode/MidCode.h"

#include "./include/StrPool.h"

StrPool strpool;

bool StrPool::_contains(const std::string& str) const {
	return _pool.count(str);
}

int StrPool::_size(void) const {
	return _pool.size();
}

void StrPool::_insert(const std::string& str) {
	if (_contains(str)) { return; }
	_pool.emplace(std::make_pair(str, 
				"str_" + std::to_string(_size())));
}

void StrPool::init(void) {
	std::set<const symtable::FuncTable*> funcs;
	SymTable::getTable().funcs(funcs);
	for (auto& functable : funcs) {
		for (auto& midcode : functable->midcodes()) {
			if (midcode->is(MidCode::Instr::OUTPUT_STR)) {
                _insert(midcode->labelName());
			}
		}
	}
}

const std::string& StrPool::operator [] (const std::string& str) const {
	return _pool.at(str);
}

extern std::ofstream mips_output;

void StrPool::output(void) const {
	for (auto& pair : _pool) {
		mips_output << pair.second << ": .asciiz \"" 
			<< pair.first << '"' << std::endl;
	}
}
