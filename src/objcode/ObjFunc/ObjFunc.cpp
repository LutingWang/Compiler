/**********************************************
    > File Name: ObjFunc.cpp
    > Author: Luting Wang
    > Mail: 2457348692@qq.com 
    > Created Time: Mon Nov 11 20:55:23 2019
 **********************************************/

#include <algorithm>
#include <cassert>
#include <fstream>
#include <set>
#include <string>
#include "midcode/MidCode.h"
#include "midcode/BasicBlock.h"
#include "midcode/FlowChart.h"
#include "symtable/table.h"
#include "symtable/Entry.h"
#include "symtable/SymTable.h"

#include "../include/ObjCode.h"
#include "../include/RegPool.h"
#include "../include/memory.h"
#include "../include/StrPool.h"

#include "./Translator.h"

#include "../include/ObjFunc.h"

std::map<std::string, ObjFunc*> ObjFunc::_func;

void ObjFunc::init(void) {
    std::set<const symtable::FuncTable*> funcs;
    SymTable::getTable().funcs(funcs);
    for (auto& functable : funcs) {
        _func[functable->name()] = new ObjFunc(functable);
    }
}

extern std::ofstream mips_output;

void ObjFunc::output(void) {
	for (auto& pair : _func) {
		mips_output << std::endl
			<< pair.first << ':' << std::endl;
        for (auto& objcode : pair.second->_objcodes) {
            objcode->output();
        }
	}
}

void ObjFunc::deinit(void) {
    for (auto& pair : _func) {
        delete pair.second;
    }
}

ObjFunc::ObjFunc(const symtable::FuncTable* const functable) {
	auto& args = functable->argList();
    
    // initialize stack frame
    std::set<const symtable::Entry*> storage;
    functable->syms(storage);
    for (auto& arg : args) {
        storage.erase(arg);
    }
    const StackFrame* const stackframe = new StackFrame(args, storage);

	// initialize register pool
	std::vector<const symtable::Entry*> reg_a(reg::a.size(), nullptr);
	for (int i = 0; i < args.size() && i < reg::a.size(); i++) {
		reg_a[i] = args[i];
	}
    RegPool* regpool = new RegPool(_objcodes, functable->midcodes(), reg_a, *stackframe);

    // prologue
	_objcodes.push_back(new ObjCode(ObjCode::Instr::subi, Reg::sp, Reg::sp, Reg::zero, stackframe->size(), ""));

    Translator translator(_objcodes, *regpool);
    FlowChart flowchart(functable);
    for (auto basicblock : flowchart.blocks()) {
        translator.compile(*basicblock);
    }
    
    delete regpool;
    delete stackframe;
}

ObjFunc::~ObjFunc(void) {
    for (auto objcode : _objcodes) {
        delete objcode;
    }
}
