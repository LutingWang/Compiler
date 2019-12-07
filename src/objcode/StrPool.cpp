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
#include "midcode.h"
#include "symtable.h"

#include "./include/StrPool.h"

StrPool strpool;

int StrPool::_size(void) const {
	return _pool.size();
}

void StrPool::_insert(const std::string& str) {
	if (_pool.count(str)) { return; }
	_pool[str] = "str$" + std::to_string(_size());
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
	for (auto& /* <str, label> */ pair : _pool) {
		mips_output << pair.second << ": .asciiz \"" 
			<< pair.first << '"' << std::endl;
	}
}
