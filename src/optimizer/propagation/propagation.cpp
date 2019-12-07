/**********************************************
    > File Name: propagation.cpp
    > Author: Luting Wang
    > Mail: 2457348692@qq.com 
    > Created Time: Thu Nov 28 19:59:03 2019
 **********************************************/

#include <set>
#include "midcode.h"
#include "symtable.h"

#include "./VarMatch.h"

#include "Optim.h"

void Optim::symProp(bool& updated) {
    std::set<symtable::FuncTable*> funcs;
    SymTable::getTable().funcs(funcs, false);
    for (auto functable : funcs) {
        // perform const propagation
        auto& midcodes = functable->_midcodes;
        for (auto it = midcodes.begin(); it < midcodes.end(); ) {
            updated = _constProp(*it) || updated;
            if (*it == nullptr) {
                it = midcodes.erase(it);
            } else {
                it++;
            }
        }
        
        // perform var propagation
        FlowChart flowchart(functable);
        for (auto basicblock : flowchart.blocks()) {
            VarMatch match;
            for (auto& midcode : basicblock->_midcodes) {
                updated = _varProp(midcode, match) || updated;
            }
        }
        flowchart.commit();
	}
}
