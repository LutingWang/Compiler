/**********************************************
    > File Name: constProp.cpp
    > Author: Luting Wang
    > Mail: 2457348692@qq.com 
    > Created Time: Fri Nov 29 15:27:29 2019
 **********************************************/

#include <cassert>
#include <functional>
#include "midcode/MidCode.h"
#include "symtable/Entry.h"

#include "Optim.h"

using Instr = MidCode::Instr;
using Arith = std::function<int(const int, const int)>;
using Cond = std::function<bool(const int, const int)>;

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
#define evalval(op) value = new int(eval(op))
#define evalcond(op) cond = new bool(eval(op));
    int* value = nullptr;
    bool* cond = nullptr;
    switch (midcode->instr()) {
    case Instr::ADD: evalval(+); break;
    case Instr::SUB: evalval(-); break;
    case Instr::MULT: evalval(*); break;
    case Instr::DIV: evalval(/); break;
    case Instr::BGT: evalcond(>); break;
    case Instr::BGE: evalcond(>=); break;
    case Instr::BLT: evalcond(<); break;
    case Instr::BLE: evalcond(<=); break;
    case Instr::BEQ: evalcond(==); break;
    case Instr::BNE: evalcond(!=); break;
    default: assert(0);
    }
    
    delete midcode;
    midcode = nullptr;
    assert((value == nullptr) != (cond == nullptr));
    if (value != nullptr) {
        midcode = new MidCode(MidCode::Instr::ASSIGN, midcode->t0(),
                MidCode::genConst(true, *value), nullptr, nullptr);
        delete value;
    } else if (*cond) {
        midcode = new MidCode(MidCode::Instr::GOTO, nullptr, nullptr,
                nullptr, new std::string(midcode->labelName()));
    }
    return true;
}
