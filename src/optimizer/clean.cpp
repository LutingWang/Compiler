/**********************************************
    > File Name: clean.cpp
    > Author: Luting Wang
    > Mail: 2457348692@qq.com 
    > Created Time: Tue Nov 19 10:10:57 2019
 **********************************************/

#include <set>
#include <string>
#include "midcode/MidCode.h"
#include "midcode/FlowChart.h"
#include "midcode/BasicBlock.h"
#include "symtable/SymTable.h"
#include "symtable/table.h"

#include "Optim.h"

void Optim::_clean(void) {
    std::set<const symtable::FuncTable*> funcs;
    const symtable::FuncTable* const mainFunc = SymTable::getTable().findFunc("main");
    FlowChart* flowchart = new FlowChart(mainFunc);
    for (auto basicblock : flowchart->blocks()) {
        if (!basicblock->isFuncCall()) {
            continue;
        }
        funcs.insert(_calledFunc(basicblock));
    }
    delete flowchart;
    flowchart = nullptr;
    funcs.erase(mainFunc);
    
    bool inserted;
    while (true) {
        inserted = false;
        for (auto functable : funcs) {
            flowchart = new FlowChart(functable);
            for (auto basicblock : flowchart->blocks()) {
                if (!basicblock->isFuncCall()) {
                    continue;
                }
                if (funcs.count(_calledFunc(basicblock))) {
                    continue;
                }
                inserted = true;
                funcs.insert(_calledFunc(basicblock));
            }
            delete flowchart;
            flowchart = nullptr;
        }
        if (!inserted) {
            break;
        }
    }
    
    auto& f = SymTable::getTable()._funcs;
    for (auto it = f.begin(); it != f.end(); ) {
        if (funcs.count(it->second)) {
            it++;
        } else {
            delete it->second;
            it = f.erase(it);
        }
    }
    
    std::set<symtable::FuncTable*> calledFuncs;
    SymTable::getTable().funcs(calledFuncs, false);
    for (auto functable : calledFuncs) {
        flowchart = new FlowChart(functable);
        for (int i = 1; i < flowchart->blocks().size(); i++) {
            auto basicblock = flowchart->blocks()[i];
            if (basicblock->_prec.size() != 0) {
                continue;
            }
            for (auto successor : basicblock->_succ) {
                successor->_prec.erase(basicblock);
            }
            for (auto midcode : basicblock->midcodes()) {
                delete midcode;
            }
            basicblock->_midcodes.clear();
            basicblock->_prec.insert(nullptr);
            i = 0;
        }
        flowchart->commit();
        delete flowchart;
        flowchart = nullptr;
    }
}
