/**********************************************
    > File Name: assignSavedRegs.cpp
    > Author: Luting Wang
    > Mail: 2457348692@qq.com 
    > Created Time: Wed Dec  4 21:17:26 2019
 **********************************************/

#include <algorithm>
#include <cassert>
#include <map>
#include "midcode/BasicBlock.h"
#include "midcode/FlowChart.h"

#include "../include/RegPool.h"

using Instr = MidCode::Instr;

namespace {
    const std::map<Instr, int> validSyms = {
        // t0|t1|t2
        { Instr::ADD, 0b111 },
        { Instr::SUB, 0b111 },
        { Instr::MULT, 0b111 },
        { Instr::DIV, 0b111 },
        { Instr::LOAD_IND, 0b101 },
        { Instr::STORE_IND, 0b011 },
        { Instr::ASSIGN, 0b110 },
        { Instr::PUSH_ARG, 0b010 },
        { Instr::CALL, 0b100 },
        { Instr::RET, 0b010 },
        { Instr::INPUT, 0b100 },
        { Instr::OUTPUT_STR, 0b000 },
        { Instr::OUTPUT_SYM, 0b010 },
        { Instr::BGT, 0b011 },
        { Instr::BGE, 0b011 },
        { Instr::BLT, 0b011 },
        { Instr::BLE, 0b011 },
        { Instr::BEQ, 0b011 },
        { Instr::BNE, 0b011 },
        { Instr::GOTO, 0b000 },
        { Instr::LABEL, 0b000 }
    };
}

void inspectBlock(std::map<const symtable::Entry*, int>& refcount, const BasicBlock* const basicblock) {
    assert(refcount.empty());
    for (auto midcode : basicblock->midcodes()) {
        const int validity = validSyms.at(midcode->instr());
        if (validity & 0b100) { refcount[midcode->t0()]++; }
        if (validity & 0b010) { refcount[midcode->t1()]++; }
        if (validity & 0b001) { refcount[midcode->t2()]++; }
    }
}

void RegPool::assignSavedRegs(const symtable::FuncTable* const functable) {
    FlowChart flowchart(functable);
    auto& blocks = flowchart.blocks();
    std::vector<int> blockGain(blocks.size(), 1);
    
    // enhance reference for blocks in loop by `LOOP_GAIN`
    static const int LOOP_GAIN = 5;
    for (int i = 0; i < blocks.size(); i++) {
        for (auto succBlock : blocks[i]->succ()) {
            // if `succBlock` shows up before `blocks[i]`, there is a loop
            for (int j = std::find(blocks.begin(), blocks.end(), succBlock) - blocks.begin();
                   j <= i; j++) {
                blockGain[j] *= LOOP_GAIN;
            }
        }
    }
    
    std::map<const symtable::Entry*, int> refcount;
    for (int i = 0; i < blocks.size(); i++) {
        std::map<const symtable::Entry*, int> tmpcount;
        inspectBlock(tmpcount, blocks[i]);
        for (auto& pair : tmpcount) {
            if (pair.first == nullptr) { continue; }
            assert(!pair.first->isArray());
            if (pair.first->isConst() || pair.first->isGlobal()) { continue; }
            refcount[pair.first] += pair.second * blockGain[i];
        }
    }
    
    assert(refcount.count(nullptr) == 0);
    
    using EntryRef = std::pair<const symtable::Entry*, int>;
    std::vector<EntryRef> sorter(refcount.begin(), refcount.end());
    std::sort(sorter.begin(), sorter.end(), [](const EntryRef& lhs, const EntryRef& rhs)
              { return lhs.second > rhs.second; });
    static const int THRESHOLD = 20;
    for (int i = 0; i < sorter.size() && i < _reg_s.size(); i++) {
        if (sorter[i].second < THRESHOLD) { break; }
        _reg_s[i] = sorter[i].first;
    }
}
