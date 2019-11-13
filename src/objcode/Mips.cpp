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
#include "./include/StrPool.h"

#include "Mips.h"

Mips Mips::__instance;

const Mips& Mips::getInstance(void) {
	return __instance;
}

Mips::~Mips(void) {
	delete _global;
	for (auto& pair : _func) {
		delete pair.second;
	}
}

void Mips::init(void) {
	assert(__instance._global == nullptr);
	strpool.init();

	std::set<symtable::Entry*> globalSyms;
	for (auto& pair : table._global._syms) {
		symtable::Entry* const entry = pair.second;
		assert(globalSyms.count(entry) == 0);
		if (entry->isConst) { continue; }
		globalSyms.insert(pair.second);
	}
	__instance._global = new Sbss(globalSyms);
	
	for (auto& pair : table._func) {
		__instance._func[pair.first] = new ObjFunc(pair.second->_midcode, pair.second->argList());
	}
	__instance._func["main"] = new ObjFunc(table._main._midcode, {});
}

extern std::ofstream mips_output;

void Mips::output(void) const {
	assert(_global != nullptr);
	mips_output << ".data" << std::endl;
	strpool.output();

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

