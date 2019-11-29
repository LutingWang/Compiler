/**********************************************
    > File Name: const.cpp
    > Author: Luting Wang
    > Mail: 2457348692@qq.com 
    > Created Time: Fri Nov 29 15:27:29 2019
 **********************************************/

#include "midcode/MidCode.h"
#include "symtable/Entry.h"

#include "Optim.h"

bool Optim::_constProp(const MidCode*& midcode) {
    if (midcode->isCalc() || midcode->isBranch()) {
        if (!midcode->t1()->isConst()) { return false; }
        if (!midcode->t2()->isConst()) { return false; }
    } else {
        return false;
    }
    
    const MidCode* newCode = nullptr;
    switch (midcode->instr()) {
#define CASE(id, op) case MidCode::Instr::id: \
        newCode = new MidCode(MidCode::Instr::ASSIGN, midcode->t0(), \
                MidCode::genConst(true, midcode->t1()->value() op \
                        midcode->t2()->value()), nullptr, nullptr); \
        break

    CASE(ADD, +); CASE(SUB, -); CASE(MULT, *); CASE(DIV, /);
#undef CASE
#define CASE(id, op) case MidCode::Instr::id: \
        if (midcode->t1()->value() op midcode->t2()->value()) { \
            newCode = new MidCode(MidCode::Instr::GOTO, nullptr, nullptr, \
                    nullptr, new std::string(midcode->labelName())); \
        } \
        break
            
    CASE(BGT, >); CASE(BGE, >=); CASE(BLT, <); CASE(BLE, <=);
    CASE(BEQ, ==); CASE(BNE, !=);
#undef CASE
    default: assert(0);
    }
    delete midcode;
    midcode = newCode;
    return true;
}
