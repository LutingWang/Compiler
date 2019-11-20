/**********************************************
    > File Name: Optim.cpp
    > Author: Luting Wang
    > Mail: 2457348692@qq.com 
    > Created Time: Tue Nov 19 10:36:03 2019
 **********************************************/

#include <set>
#include "midcode/MidCode.h"
#include "midcode/BasicBlock.h"
#include "symtable/table.h"
#include "symtable/SymTable.h"

#include "Optim.h"

const symtable::FuncTable* Optim::_calledFunc(const BasicBlock* const basicblock) {
    return SymTable::getTable().findFunc(basicblock->midcodes().back()->labelName());
}

void Optim::peephole(void) {
    std::set<symtable::FuncTable*> funcs;
    SymTable::getTable().funcs(funcs, false);
    for (auto functable : funcs) {
        for (auto it = functable->_midcodes.begin(); it != functable->_midcodes.end() - 1; ) {
            if ((*it)->is(MidCode::Instr::GOTO) && (*(it + 1))->is(MidCode::Instr::LABEL) && (*it)->labelName() == (*(it + 1))->labelName()) {
                it = functable->_midcodes.erase(it);
            } else {
                it++;
            }
        }
    }
}
