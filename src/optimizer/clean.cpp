/**********************************************
    > File Name: clean.cpp
    > Author: Luting Wang
    > Mail: 2457348692@qq.com 
    > Created Time: Tue Nov 19 10:10:57 2019
 **********************************************/

#include <algorithm>
#include <cassert>
#include <set>
#include <string>
#include <vector>
#include "midcode/MidCode.h"
#include "midcode/FlowChart.h"
#include "midcode/BasicBlock.h"
#include "symtable.h"

#include "Optim.h"

void Optim::_cleanFuncs(void) {
    // bfs search all called functions
    std::set<const symtable::FuncTable*> funcs;
    std::vector<const symtable::FuncTable*> queue { SymTable::getTable()._main };
    for (int i = 0; i < queue.size(); i++) {
        for (auto midcode : queue[i]->midcodes()) {
            if (!midcode->is(MidCode::Instr::CALL)) { continue; }
            const symtable::FuncTable* const functable = SymTable::getTable().findFunc(midcode->labelName());
            auto result = funcs.insert(functable);
            if (result.second) { queue.push_back(functable); }
        }
    }
    
    // delete functions never called
    auto& f = SymTable::getTable()._funcs;
    for (auto it = f.begin(); it != f.end(); ) {
        if (funcs.count(it->second)) {
            it++;
        } else {
            // FIXME: memory leak of syms in `it->second`
            delete it->second;
            it = f.erase(it);
        }
    }
}

void Optim::_cleanBlocks(symtable::FuncTable* const functable) {
    FlowChart flowchart(functable);
    for (int i = 1; i < flowchart.blocks().size(); i++) {
        auto basicblock = flowchart.blocks()[i];
        
        // skip basic blocks that can be reached
        if (basicblock->_prec.size() != 0) { continue; }
        
        // successors cannot be reached through this block
        for (auto successor : basicblock->_succ) {
            successor->_prec.erase(basicblock);
        }
        
        // delete all the midcodes so that they won't be written back
        for (auto midcode : basicblock->midcodes()) {
            delete midcode;
        }
        basicblock->_midcodes.clear();
        
        // pretend to be reachable to avoid dead loop
        basicblock->_prec.insert(nullptr);
        
        // restart scanning from begin
        i = 0;
    }
    flowchart.commit();
}

void Optim::_cleanLabels(symtable::FuncTable* const functable) {
    std::set<std::string> usedLabels;
    for (auto midcode : functable->midcodes()) {
        if (midcode->isBranch() || midcode->is(MidCode::Instr::GOTO)) {
            usedLabels.insert(midcode->labelName());
        }
    }
    
    auto& midcodes = functable->_midcodes;
    for (auto it = midcodes.begin(); it != midcodes.end(); ) {
        if ((*it)->is(MidCode::Instr::LABEL) && usedLabels.count((*it)->labelName()) == 0) {
            it = midcodes.erase(it);
        } else { it++; }
    }
}

void Optim::clean(void) {
    _cleanFuncs();
    std::set<symtable::FuncTable*> funcs;
    SymTable::getTable().funcs(funcs, false);
    for (auto functable : funcs) {
        _cleanBlocks(functable);
        _cleanLabels(functable);
    }
}
