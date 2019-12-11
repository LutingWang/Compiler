/**********************************************
    > File Name: datastream.cpp
    > Author: Luting Wang
    > Mail: 2457348692@qq.com 
    > Created Time: Thu Dec  5 10:30:24 2019
 **********************************************/

#include <algorithm>
#include <cassert>
#include "midcode.h"
#include "symtable.h"

#include "datastream.h"

#define FILTER(midcode, t) (midcode->t##IsValid() && \
    !(midcode->t()->isGlobal() || midcode->t()->isConst() || midcode->t()->isArray()))

/* LiveVar */

void LiveVar::use(std::vector<const symtable::Entry*>& output, const MidCode* const midcode) {
    assert(output.empty());
    if (FILTER(midcode, t1)) { output.push_back(midcode->t1()); }
    if (FILTER(midcode, t2)) { output.push_back(midcode->t2()); }
}

void LiveVar::def(symtable::Entry const*& output, const MidCode* const midcode) {
    assert(output == nullptr);
    if (FILTER(midcode, t0)) { output = midcode->t0(); }
}

void LiveVar::analyze(Vars& useList, Vars& defList, const BasicBlock* const basicblock) {
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

LiveVar::LiveVar(const FlowChart& flowchart) {
    auto& blocks = flowchart.blocks();
    std::vector<Vars> in(blocks.size()); // initiated as use
    std::vector<Vars> def(blocks.size());
    
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

void LiveVar::backProp(std::vector<Vars>& output, const BasicBlock* const basicblock) const {
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

/* ReachDef */

using DefMap = std::map<const symtable::Entry*, const MidCode*>;

void ReachDef::gen(DefMap& output, const BasicBlock* const basicblock) {
    assert(output.empty());
    for (auto midcode : basicblock->midcodes()) {
        if (!FILTER(midcode, t0)) { continue; }
        output[midcode->t0()] = midcode;
    }
}

void ReachDef::kill(Defs& output, const DefMap& blockGen,
        const std::map<const symtable::Entry*, Defs>& funcGen) {
    assert(output.empty());
    for (auto& /* <entry, midcode> */ pair : blockGen) {
        auto& killList = funcGen.at(pair.first);
        output.insert(killList.begin(), killList.end());
    }
}

void ReachDef::analyze(std::vector<Defs>& genList, std::vector<Defs>& killList,
        const std::vector<BasicBlock*>& blocks) {
    // map version of gen for each block
    std::vector<DefMap> blockGens(blocks.size());
    for (int i = 0; i < blocks.size(); i++) {
        gen(blockGens[i], blocks[i]);
    }
    
    // initialize funcGen and genList (the set version of gen) in the same loop
    std::map<const symtable::Entry*, Defs> funcGen;
    for (int i = 0; i < blocks.size(); i++) {
        for (auto& /* <entry, midcode> */ pair : blockGens[i]) {
            funcGen[pair.first].insert(pair.second);
            genList[i].insert(pair.second);
        }
    }
    
    // initialize killList
    for (int i = 0; i < blocks.size(); i++) {
        kill(killList[i], blockGens[i], funcGen);
    }
}

ReachDef::ReachDef(const FlowChart& flowchart) {
    auto& blocks = flowchart.blocks();
    std::vector<Defs> out(blocks.size()); // initiated as gen
    std::vector<Defs> kill(blocks.size());
    analyze(out, kill, blocks);
    
    for (bool updated = true; updated; ) {
        updated = false;
        for (int i = 0; i < blocks.size(); i++) {
            auto& in = _in[blocks[i]];

            // in = \cup_{prec} out[ind(prec)]
            for (auto prec : blocks[i]->prec()) {
                int ind = std::find(blocks.begin(), blocks.end(), prec) - blocks.begin();
                in.insert(out[ind].begin(), out[ind].end());
            }

            // out = out \cup (in - kill)
            for (auto midcode : in) {
                if (kill[i].count(midcode) != 0) { continue; }
                auto result = out[i].insert(midcode);
                updated = updated || result.second;
            }
        }
    }
}

void ReachDef::getIn(Defs& output, const BasicBlock* const basicblock) const {
    output = _in.at(basicblock);
}
