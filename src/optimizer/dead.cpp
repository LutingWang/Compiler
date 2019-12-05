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
#include "midcode/BasicBlock.h"
#include "midcode/FlowChart.h"
#include "symtable/Entry.h"
#include "symtable/table.h"
#include "symtable/SymTable.h"

#include "Optim.h"

bool Optim::deadCodeDel(void) {
    bool result = false;
    std::set<symtable::FuncTable*> funcs;
    SymTable::getTable().funcs(funcs, false);
    for (auto functable : funcs) {
        FlowChart flowchart(functable);
        LiveVar livevar(flowchart);
        for (auto basicblock : flowchart.blocks()) {
            std::vector<std::set<const symtable::Entry*>> out;
            livevar.backProp(out, basicblock);
            assert(out.size() == basicblock->midcodes().size());
            for (int i = 0; i < out.size(); ) {
                const MidCode* const midcode = basicblock->midcodes()[i];
                const symtable::Entry* defSym = nullptr;
                LiveVar::def(defSym, midcode);
                
                assert(defSym == nullptr || (!defSym->isConst() && !defSym->isArray()));
                if (defSym == nullptr || defSym->isGlobal() || // global vars are not included
                        out[i].count(defSym) != 0) { // defSym is alive
                    i++;
                    continue;
                }
                
                result = true;
                out.erase(out.begin() + i);
                basicblock->_midcodes.erase(basicblock->_midcodes.begin() + i);
            }
        }
        flowchart.commit();
    }
    return result;
}
