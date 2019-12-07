/**********************************************
    > File Name: Optim.cpp
    > Author: Luting Wang
    > Mail: 2457348692@qq.com 
    > Created Time: Tue Nov 19 10:36:03 2019
 **********************************************/

#include <cassert>
#include <set>
#include "midcode.h"
#include "symtable.h"

#include "Optim.h"

void Optim::peephole(bool& updated) {
    std::set<symtable::FuncTable*> funcs;
    SymTable::getTable().funcs(funcs, false);
    for (auto functable : funcs) {
        for (auto it = functable->_midcodes.begin(); 
				it != functable->_midcodes.end() - 1; ) {
            if ((*it)->is(MidCode::Instr::GOTO) && 
					(*(it + 1))->is(MidCode::Instr::LABEL) && 
					(*it)->labelName() == (*(it + 1))->labelName()) {
                it = functable->_midcodes.erase(it);
                updated = true;
            } else {
                it++;
            }
        }
    }
}
