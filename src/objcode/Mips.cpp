/**********************************************
    > File Name: Mips.cpp
    > Author: Luting Wang
    > Mail: 2457348692@qq.com 
    > Created Time: Mon Nov 11 15:40:01 2019
 **********************************************/

#include <algorithm>
#include <fstream>
#include <map>
#include <string>
#include "symtable.h"

#include "./include/ObjFunc.h"
#include "./include/StackFrame.h"

#include "Mips.h"

Mips Mips::_instance;

Mips& Mips::getInstance(void) {
	return _instance;
}

Mips::~Mips(void) {
	delete _global;
	for (auto& pair : _func) {
		delete pair.second;
	}
}

extern std::ofstream mips_output;

void Mips::_output(void) const {
	mips_output << ".data" << std::endl;
	for (auto& pair : _str) {
		mips_output << pair.second << ": .asciiz \"" 
			<< pair.first << '"' << std::endl;
	}

	mips_output << std::endl 
		<< ".text" << std::endl
		<< "jal main" << std::endl
		<< "li $v0 10" << std::endl
		<< "syscall" << std::endl;

	for (auto& pair : _func) {
		mips_output << std::endl
			<< pair.first << ':' << std::endl;
		pair.second->output();
	}
}

void Mips::output(void) {
	if (_global != nullptr) {
		_output();
		return;
	}

	std::set<symtable::Entry*> globalSyms;
	for (auto& pair : table._global._syms) {
		symtable::Entry* const entry = pair.second;
		assert(globalSyms.count(entry) == 0);
		if (entry->isConst) { continue; }
		globalSyms.insert(pair.second);
	}
	_global = new Sbss(globalSyms);
	
	for (auto& pair : table._func) {
		_func[pair.first] = new ObjFunc(pair.second->_midcode, pair.second->argList());
	}
	_func["main"] = new ObjFunc(table._main._midcode, {});

	_output();
}
