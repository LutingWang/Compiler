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
#include "midcode.h"
#include "symtable.h"

#include "../include/ObjCode.h"
#include "../include/RegPool.h"
#include "../include/memory.h"

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
	for (auto& /* <funcName, objfunc> */ pair : _func) {
		mips_output << std::endl
			<< pair.first << ':' << std::endl;
        for (auto& objcode : pair.second->_objcodes) {
            objcode->output();
        }
	}
}

void ObjFunc::deinit(void) {
    for (auto& /* <funcName, objfunc> */ pair : _func) {
        delete pair.second;
    }
}

ObjFunc::ObjFunc(const symtable::FuncTable* const functable) {
    // call back function to insert objcode
    const objcode::CodeGen output =
        [this](const objcode::ObjCode* const objcode) {
            this->_objcodes.push_back(objcode);
        };
    
	auto& args = functable->argList();
    
    // initialize stackframe
    std::set<const symtable::Entry*> storage;
    functable->syms(storage);
    for (auto& arg : args) {
        storage.erase(arg);
    }
    const StackFrame stackframe(output, args, storage);

	// initialize register pool
    RegPool regpool(functable, stackframe);
    
    // prologue
    output(objcode::SubFactory::produce(Reg::sp, Reg::sp, stackframe.size()));
    regpool.genPrologue();

    // start translation
    const FlowChart flowchart(functable);
    Translator translator(output, regpool, stackframe);
    for (auto basicblock : flowchart.blocks()) {
        translator.compile(basicblock);
    }
}

ObjFunc::~ObjFunc(void) {
    for (auto objcode : _objcodes) {
        delete objcode;
    }
}
