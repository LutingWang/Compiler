/**********************************************
    > File Name: constProp.cpp
    > Author: Luting Wang
    > Mail: 2457348692@qq.com 
    > Created Time: Fri Nov 29 15:27:29 2019
 **********************************************/

#include <cassert>
#include "midcode.h"
#include "symtable.h"

#include "Optim.h"

using Instr = MidCode::Instr;

bool Optim::_constProp(const MidCode*& midcode) {
    // store ind cannot be optimized
    if (midcode->is(Instr::STORE_IND)) {
        return false;
    }
    if (!midcode->t1IsValid() || !midcode->t2IsValid()) {
        return false;
    }
    if (!midcode->t1()->isConst() ||!midcode->t2()->isConst()) {
        return false;
    }
    
#define eval(op) (midcode->t1()->value() op midcode->t2()->value())
    int* value = nullptr;
    bool* cond = nullptr;
    switch (midcode->instr()) {
    case Instr::ADD: value = new int(eval(+)); break;
    case Instr::SUB: value = new int(eval(-)); break;
    case Instr::MULT: value = new int(eval(*)); break;
    case Instr::DIV: value = new int(eval(/)); break;
    case Instr::BGT: cond = new bool(eval(>)); break;
    case Instr::BGE: cond = new bool(eval(>=)); break;
    case Instr::BLT: cond = new bool(eval(<)); break;
    case Instr::BLE: cond = new bool(eval(<=)); break;
    case Instr::BEQ: cond = new bool(eval(==)); break;
    case Instr::BNE: cond = new bool(eval(!=)); break;
    default: assert(0);
    }
    
    const MidCode* newCode = nullptr;
    assert((value == nullptr) != (cond == nullptr));
    if (value != nullptr) {
        newCode = new MidCode(MidCode::Instr::ASSIGN, midcode->t0(),
                MidCode::genConst(true, *value), nullptr, nullptr);
    } else if (*cond) {
        newCode = new MidCode(MidCode::Instr::GOTO, nullptr, nullptr,
                nullptr, new std::string(midcode->labelName()));
    }
    delete value;
    delete cond;
    delete midcode;
    midcode = newCode;
    return true;
}
