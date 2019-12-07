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
#include "datastream/LiveVar.h"
#include "midcode/MidCode.h"
#include "midcode/BasicBlock.h"
#include "midcode/FlowChart.h"
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
    CodeGen output = [this](const ObjCode::Instr instr,
            const Reg t0, const Reg t1, const Reg t2,
            const int imm, const std::string& label) {
        this->_objcodes.push_back(
                new ObjCode(instr, t0, t1, t2, imm, label));
    };
    
	auto& args = functable->argList();
    
    // initialize helpers
    const FlowChart flowchart(functable);
    // LiveVar livevar(flowchart);
    
    // initialize stackframe
    std::set<const symtable::Entry*> storage;
    functable->syms(storage);
    for (auto& arg : args) {
        storage.erase(arg);
    }
    const StackFrame stackframe(output, args, storage);

	// initialize register pool
	std::vector<const symtable::Entry*> reg_a(reg::a.size(), nullptr);
	for (int i = 0; i < args.size() && i < reg::a.size(); i++) {
		reg_a[i] = args[i];
	}
    RegPool regpool(reg_a, stackframe);
    // TODO: uncomment
    // regpool.assignSavedRegs(functable);
    
    // prologue
    output(ObjCode::Instr::subi, Reg::sp, Reg::sp, ObjCode::noreg, stackframe.size(), ObjCode::nolab);
    regpool.genPrologue();

    // start translation
    Translator translator(output, regpool, stackframe);
    for (auto basicblock : flowchart.blocks()) {
        translator.compile(*basicblock);
    }
}

ObjFunc::~ObjFunc(void) {
    for (auto objcode : _objcodes) {
        delete objcode;
    }
}
