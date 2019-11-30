/**********************************************
    > File Name: propagation.cpp
    > Author: Luting Wang
    > Mail: 2457348692@qq.com 
    > Created Time: Thu Nov 28 19:59:03 2019
 **********************************************/

#include <set>
#include "midcode/MidCode.h"
#include "midcode/BasicBlock.h"
#include "midcode/FlowChart.h"
#include "symtable/table.h"
#include "symtable/SymTable.h"

#include "./VarMatch.h"

#include "Optim.h"

bool Optim::symProp(void) {
    bool result = false;
    
    std::set<symtable::FuncTable*> funcs;
    SymTable::getTable().funcs(funcs, false);
    for (auto functable : funcs) {
        bool updated = true;
        while (updated) {
            updated = false;
            
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
            
            // some midcodes are changed
            if (updated) { result = true; }
        }
	}
    return result;
}
