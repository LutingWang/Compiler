/**********************************************
    > File Name: peephole.cpp
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
        // label of goto is the next midcode
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
        
        // t = b + c; a = t;
        for (auto it = functable->_midcodes.begin() + 1;
                it != functable->_midcodes.end(); it++) {
            if (!(*it)->is(MidCode::Instr::ASSIGN)) { continue; }
            const MidCode* const prev = *(it - 1);
            if (prev->isCalc()) {
                // self changing code cannot be peepholed
                if (prev->t0() == prev->t1()) { continue; }
                else if (prev->t0() == prev->t2()) { continue; }
            } else if (!prev->is(MidCode::Instr::ASSIGN)) {
                continue;
            }
            if (prev->t0() != (*it)->t1()) { continue; }
            const symtable::Entry* target = (*it)->t0();
            delete *it;
            *it = new MidCode(prev->instr(), target, prev->_t1, prev->_t2, nullptr);
        }
    }
}
