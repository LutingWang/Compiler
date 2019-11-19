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
#include "symtable/table.h"
#include "symtable/SymTable.h"

#include "Optim.h"

void Optim::inlineExpan(void) {
    std::set<symtable::FuncTable*> funcs;
    SymTable::getTable().funcs(funcs, false);
    for (auto& functable : funcs) {
        bool replaced;
        while (true) {
            replaced = false;
            FlowChart flowchart(functable);
            for (auto basicblock : flowchart.blocks()) {
                if (!basicblock->isFuncCall() || !SymTable::getTable().findFunc(basicblock->midcodes().back()->labelName())->isInline()) {
                    continue;
                }
                replaced = true;
                const symtable::FuncTable* const calledFunc = _calledFunc(basicblock);
                
                // iterate over all push statements and convert to assign
                for (int i = 0; i < basicblock->midcodes().size() - 1; i++) {
                    const MidCode* const midcode = basicblock->midcodes()[i];
                    assert (!midcode->is(MidCode::Instr::CALL));
                    const MidCode* const assign = new MidCode(MidCode::Instr::ASSIGN, calledFunc->argList()[i], midcode->t1(), nullptr, nullptr);
                    delete midcode;
                    basicblock->_midcodes[i] = assign;
                }
                
                // retrieve sym to save return value
                const symtable::Entry* const retsym = basicblock->midcodes().back()->t0();
                assert((retsym == nullptr) == (calledFunc->isVoid()));
                delete basicblock->midcodes().back();
                basicblock->_midcodes.pop_back();
                
                // deep clone the mid code
                std::string end = MidCode::genLabel();
                for (auto midcode : calledFunc->midcodes()) {
                    if (midcode->is(MidCode::Instr::RET)) {
                        if (retsym != nullptr) {
                            basicblock->_midcodes.push_back(new MidCode(MidCode::Instr::ASSIGN, retsym, midcode->t1(), nullptr, nullptr));
                        }
                        basicblock->_midcodes.push_back(new MidCode(MidCode::Instr::GOTO, nullptr, nullptr, nullptr, new std::string(end)));
                    } else {
                        basicblock->_midcodes.push_back(new MidCode(*midcode));
                    }
                }
                basicblock->_midcodes.push_back(new MidCode(MidCode::Instr::LABEL, nullptr, nullptr, nullptr, new std::string(end)));
                
                // deep clone syms
                *functable << *calledFunc;
            }
            if (replaced) {
                flowchart.commit();
            } else {
                break;
            }
        }
    }
    _clean();
}
