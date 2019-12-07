/**********************************************
    > File Name: dead.cpp
    > Author: Luting Wang
    > Mail: 2457348692@qq.com 
    > Created Time: Thu Dec  5 18:20:29 2019
 **********************************************/

#include <cassert>
#include <set>
#include <vector>
#include "datastream/LiveVar.h"
#include "midcode.h"
#include "symtable.h"

#include "Optim.h"

void Optim::deadCodeDel(bool& updated) {
    std::set<symtable::FuncTable*> funcs;
    SymTable::getTable().funcs(funcs, false);
    for (auto functable : funcs) {
        FlowChart flowchart(functable);
        const LiveVar livevar(flowchart);
        for (auto basicblock : flowchart.blocks()) {
            std::vector<std::set<const symtable::Entry*>> out;
            livevar.backProp(out, basicblock);
            assert(out.size() == basicblock->midcodes().size());
            for (int i = 0; i < out.size(); ) {
                const MidCode* const midcode = basicblock->midcodes()[i];
                const symtable::Entry* defSym = nullptr;
                LiveVar::def(defSym, midcode);
                
                assert(defSym == nullptr || (!defSym->isConst() && !defSym->isArray()));
                if (defSym == nullptr || // no output variable
                    midcode->is(MidCode::Instr::CALL) || // global could change in a call
                    midcode->is(MidCode::Instr::INPUT) || // input cannot be changed
                    defSym->isGlobal() || // global vars are not included
                    out[i].count(defSym) != 0) { // defSym is alive
                    i++;
                    continue;
                }
                
                updated = true;
                out.erase(out.begin() + i);
                basicblock->_midcodes.erase(basicblock->_midcodes.begin() + i);
            }
        }
        flowchart.commit();
    }
}
