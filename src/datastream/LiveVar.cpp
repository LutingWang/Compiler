/**********************************************
    > File Name: LiveVar.cpp
    > Author: Luting Wang
    > Mail: 2457348692@qq.com 
    > Created Time: Thu Dec  5 10:30:24 2019
 **********************************************/

#include <algorithm>
#include <cassert>
#include "midcode/MidCode.h"
#include "midcode/BasicBlock.h"
#include "midcode/FlowChart.h"
#include "symtable/Entry.h"

#include "datastream/LiveVar.h"

bool isGlobalOrConst(const symtable::Entry* const entry) {
    return entry->isGlobal() || entry->isConst();
}

LiveVar::LiveVar(const FlowChart& flowchart) {
    auto& blocks = flowchart.blocks();
    std::vector<std::set<const symtable::Entry*>> in(blocks.size()); // initiated as use
    std::vector<std::set<const symtable::Entry*>> def(blocks.size());
    
    for (int i = 0; i < blocks.size(); i++) {
        auto& useList = in[i];
        auto& defList = def[i];
        analyze(useList, defList, blocks[i]);
        
        // remove global or const
        for (auto it = useList.begin(); it != useList.end(); ) {
            if (isGlobalOrConst(*it)) {
                it = useList.erase(it);
            } else { it++; }
        }
        
        // no const permitted, so remove global only
        for (auto it = defList.begin(); it != defList.end(); ) {
            assert(!(*it)->isConst());
            if (isGlobalOrConst(*it)) {
                it = defList.erase(it);
            } else { it++; }
        }
    }
    
    for (bool updated = true; updated; ) {
        updated = false;
        for (int i = 0; i < blocks.size(); i++) {
            auto& out = _out[blocks[i]];
            
            // out = \cup_{succ} in[ind(succ)]
            for (auto succ : blocks[i]->succ()) {
                int ind = std::find(blocks.begin(), blocks.end(), succ) - blocks.begin();
                if (ind == blocks.size()) { continue; } // succ is the exit block where in = \phi
                out.insert(in[ind].begin(), in[ind].end());
            }
            
            // in = in \cup (out - def)
            for (auto entry : out) {
                if (def[i].count(entry) != 0) { continue; }
                auto result = in[i].insert(entry);
                updated = updated || result.second;
            }
        }
    }
}

void LiveVar::backProp(std::vector<std::set<const symtable::Entry*>>& output, const BasicBlock* const basicblock) {
    assert(output.empty());
    output.resize(basicblock->midcodes().size());
    auto out = _out[basicblock];
    for (int i = output.size() - 1; i >= 0; i--) {
        output[i] = out;
        const MidCode* const midcode = basicblock->midcodes()[i];
        
        const symtable::Entry* defSym = nullptr;
        def(defSym, midcode);
        out.erase(defSym);
        
        std::vector<const symtable::Entry*> useSym;
        use(useSym, midcode);
        if (useSym.size() >= 1 && !isGlobalOrConst(useSym[0])) {
            out.insert(useSym[0]);
        }
        if (useSym.size() >= 2 && !isGlobalOrConst(useSym[1])) {
            out.insert(useSym[1]);
        }
    }
}

void LiveVar::use(std::vector<const symtable::Entry*>& output, const MidCode* const midcode) {
    assert(output.empty());
    // note that t1 could be array in `LOAD_IND`
    if (midcode->t1IsValid() && !midcode->t1()->isArray()) {
        output.push_back(midcode->t1());
    }
    if (midcode->t2IsValid()) {
        output.push_back(midcode->t2());
    }
}

void LiveVar::def(symtable::Entry const*& output, const MidCode* const midcode) {
    assert(output == nullptr);
    // note that t0 could be array in `STORE_IND`
    if (midcode->t0IsValid() && !midcode->t0()->isArray()) {
        output = midcode->t0();
    }
}

void LiveVar::analyze(std::set<const symtable::Entry*>& useList,
        std::set<const symtable::Entry*>& defList,
        const BasicBlock* const basicblock) {
    assert(useList.empty());
    assert(defList.empty());
    
    for (auto midcode : basicblock->midcodes()) {
        std::vector<const symtable::Entry*> codeUse;
        use(codeUse, midcode);
        if (codeUse.size() >= 1 && defList.count(codeUse[0]) == 0) { useList.insert(codeUse[0]); }
        if (codeUse.size() >= 2 && defList.count(codeUse[1]) == 0) { useList.insert(codeUse[1]); }
        
        const symtable::Entry* codeDef = nullptr;
        def(codeDef, midcode);
        if (codeDef != nullptr && useList.count(codeDef) == 0) { defList.insert(codeDef); }
    }
}
