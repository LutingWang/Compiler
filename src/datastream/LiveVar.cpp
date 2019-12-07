/**********************************************
    > File Name: LiveVar.cpp
    > Author: Luting Wang
    > Mail: 2457348692@qq.com 
    > Created Time: Thu Dec  5 10:30:24 2019
 **********************************************/

#include <algorithm>
#include <cassert>
#include "midcode.h"
#include "symtable.h"

#include "datastream/LiveVar.h"

LiveVar::LiveVar(const FlowChart& flowchart) {
    auto& blocks = flowchart.blocks();
    std::vector<std::set<const symtable::Entry*>> in(blocks.size()); // initiated as use
    std::vector<std::set<const symtable::Entry*>> def(blocks.size());
    
    for (int i = 0; i < blocks.size(); i++) {
        auto& useList = in[i];
        auto& defList = def[i];
        analyze(useList, defList, blocks[i]);
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

void LiveVar::backProp(std::vector<std::set<const symtable::Entry*>>& output,
        const BasicBlock* const basicblock) const {
    assert(output.empty());
    output.resize(basicblock->midcodes().size());
    auto out = _out.at(basicblock);
    for (int i = output.size() - 1; i >= 0; i--) {
        output[i] = out;
        const MidCode* const midcode = basicblock->midcodes()[i];
        
        const symtable::Entry* defSym = nullptr;
        def(defSym, midcode);
        out.erase(defSym);
        
        std::vector<const symtable::Entry*> useSym;
        use(useSym, midcode);
        out.insert(useSym.begin(), useSym.end());
    }
}

void LiveVar::use(std::vector<const symtable::Entry*>& output, const MidCode* const midcode) {
    assert(output.empty());
    if (FILTER(midcode, t1)) { output.push_back(midcode->t1()); }
    if (FILTER(midcode, t2)) { output.push_back(midcode->t2()); }
}

void LiveVar::def(symtable::Entry const*& output, const MidCode* const midcode) {
    assert(output == nullptr);
    if (FILTER(midcode, t0)) { output = midcode->t0(); }
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
