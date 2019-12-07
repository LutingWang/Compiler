/**********************************************
    > File Name: inline.cpp
    > Author: Luting Wang
    > Mail: 2457348692@qq.com 
    > Created Time: Wed Nov  6 22:13:52 2019
 **********************************************/

#include <cassert>
#include <set>
#include <string>
#include <vector>
#include "midcode/MidCode.h"
#include "midcode/BasicBlock.h"
#include "midcode/FlowChart.h"
#include "symtable.h"

#include "Optim.h"

const symtable::FuncTable* Optim::_repaceBlock(BasicBlock* const basicblock) {
    if (!basicblock->isFuncCall()) { return nullptr; }
    const symtable::FuncTable* const calledFunc = SymTable::getTable().findFunc(
            basicblock->midcodes().back()->labelName());
    if (!calledFunc->isInline()) { return nullptr; }
    
    // iterate over all push statements and convert to assign
    for (int i = 0; i < basicblock->midcodes().size() - 1; i++) {
        const MidCode* const midcode = basicblock->midcodes()[i];
        assert (!midcode->is(MidCode::Instr::CALL));
        const MidCode* const assign = new MidCode(MidCode::Instr::ASSIGN,
                    calledFunc->argList()[i], midcode->t1(), nullptr, nullptr);
        delete midcode;
        basicblock->_midcodes[i] = assign;
    }
    
    // retrieve the sym to save return value
    const symtable::Entry* const retsym = basicblock->midcodes().back()->t0();
    assert((retsym == nullptr) == (calledFunc->isVoid()));
    delete basicblock->midcodes().back();
    basicblock->_midcodes.pop_back();
    
    // deep clone the midcode
    std::string end = MidCode::genLabel();
    std::map<std::string, std::string> labelMap;
    for (auto midcode : calledFunc->midcodes()) {
        switch (midcode->instr()) {
        case MidCode::Instr::RET:
            if (retsym != nullptr) {
                basicblock->_midcodes.push_back(new MidCode(MidCode::Instr::ASSIGN,
                        retsym, midcode->t1(), nullptr, nullptr));
            }
            basicblock->_midcodes.push_back(new MidCode(MidCode::Instr::GOTO,
                    nullptr, nullptr, nullptr, new std::string(end)));
            break;
        case MidCode::Instr::BGT:
        case MidCode::Instr::BGE:
        case MidCode::Instr::BLT:
        case MidCode::Instr::BLE:
        case MidCode::Instr::BEQ:
        case MidCode::Instr::BNE:
        case MidCode::Instr::GOTO:
        case MidCode::Instr::LABEL:
            if (labelMap.count(midcode->labelName()) == 0) {
                labelMap[midcode->labelName()] = MidCode::genLabel();
            }
            basicblock->_midcodes.push_back(new MidCode(midcode->_instr, midcode->_t0,
                    midcode->_t1, midcode->_t2, new std::string(labelMap.at(midcode->labelName()))));
            break;
        default:
            basicblock->_midcodes.push_back(new MidCode(*midcode));
        }
    }
    basicblock->_midcodes.push_back(new MidCode(MidCode::Instr::LABEL, nullptr,
            nullptr, nullptr, new std::string(end)));
    
    return calledFunc;
}

bool Optim::inlineExpan(void) {
    bool result = false;
    
    std::set<symtable::FuncTable*> funcs;
    SymTable::getTable().funcs(funcs, false);
    for (auto functable : funcs) {
        for (bool replaced = true; replaced; ) {
            replaced = false;
            FlowChart flowchart(functable);
            for (auto basicblock : flowchart.blocks()) {
                const symtable::FuncTable* const calledFunc = _repaceBlock(basicblock);
                if (calledFunc != nullptr) {
                    replaced = true;
                    *functable << *calledFunc;
                    
                    // Commit immediately after replacement. Otherwise, compiler
                    // may break down if the following processes checks whether
                    // this function is recursive.
                    flowchart.commit();
                }
            }
            if (replaced) { result = true; }
        }
    }
    return result;
}
