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
        
        // preparation for var propagation
        FlowChart flowchart(functable);
        auto& blocks = flowchart.blocks();
        std::vector<VarMatch*> matches(blocks.size());
        ReachDef reachdef(flowchart);
        for (int i = 0; i < blocks.size(); i++) {
            Defs defs;
            reachdef.getIn(defs, blocks[i]);
            matches[i] = new VarMatch(defs);
        }
        
        // perform var propagation
        for (int i = 0; i < blocks.size(); i++) {
            for (auto& midcode : blocks[i]->_midcodes) {
                updated = _varProp(midcode, *(matches[i])) || updated;
            }
        }
        
        // deinit matches
        for (int i = 0; i < matches.size(); i++) {
            delete matches[i];
        }
        
        flowchart.commit();
	}
}
