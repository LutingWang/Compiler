/**********************************************
    > File Name: compileCore.cpp
    > Author: Luting Wang
    > Mail: 2457348692@qq.com 
    > Created Time: Wed Dec  4 10:50:46 2019
 **********************************************/

#include <map>
#include "../include/memory.h"
#include "../include/StrPool.h"

#include "./Translator.h"

namespace {
    auto& noreg =  ObjCode::noreg;
    auto& noimm =  ObjCode::noimm;
    auto& nolab =  ObjCode::nolab;

    const std::map<MidCode::Instr, ObjCode::Instr> instrMap = {
        { MidCode::Instr::ADD, ObjCode::Instr::add },
        { MidCode::Instr::SUB, ObjCode::Instr::sub },
        { MidCode::Instr::MULT, ObjCode::Instr::mul },
        { MidCode::Instr::BGT, ObjCode::Instr::bgt },
        { MidCode::Instr::BGE, ObjCode::Instr::bge },
        { MidCode::Instr::BLT, ObjCode::Instr::blt },
        { MidCode::Instr::BLE, ObjCode::Instr::ble },
        { MidCode::Instr::BEQ, ObjCode::Instr::beq },
        { MidCode::Instr::BNE, ObjCode::Instr::bne }
    };
}

#define GEN(id, t0, t1, t2, imm, lab) \
    _output(ObjCode::Instr::id, t0, t1, t2, imm, lab);
#define REQ _regpool.request()

void Translator::_compileCode(const MidCode& midcode) {
    const StackFrame& _stackframe = _regpool.stackframe();
    Reg t0, t1, t2;
    switch (midcode.instr()) {
    case MidCode::Instr::ADD:
    case MidCode::Instr::SUB:
    case MidCode::Instr::MULT:
        t1 = REQ;
        t2 = REQ;
        t0 = REQ;
        _output(instrMap.at(midcode.instr()), t0, t1, t2, noimm, nolab);
        break;
    case MidCode::Instr::DIV:
        t1 = REQ;
        t2 = REQ;
        GEN(div, noreg, t1, t2, noimm, nolab);
        t0 = REQ;
        GEN(mflo, t0, noreg, noreg, noimm, nolab);
        break;
    case MidCode::Instr::LOAD_IND:
        t2 = REQ;
        GEN(sll, Reg::t8, t2, noreg, 2, nolab);
        GEN(add, Reg::t8, Reg::t8, midcode.t1()->isGlobal() ? Reg::gp : Reg::sp, noimm, nolab);
        t0 = REQ;
        GEN(lw, t0, Reg::t8, noreg, _stackframe[midcode.t1()], nolab);
        break;
    case MidCode::Instr::STORE_IND:
        t2 = REQ;
        GEN(sll, Reg::t8, t2, noreg, 2, nolab);
        GEN(add, Reg::t8, Reg::t8, midcode.t0()->isGlobal() ? Reg::gp : Reg::sp, noimm, nolab);
        t1 = REQ;
        GEN(sw, t1, Reg::t8, noreg, _stackframe[midcode.t0()], nolab);
        break;
    case MidCode::Instr::ASSIGN:
        t1 = REQ;
        t0 = REQ;
        GEN(move, t0, t1, noreg, noimm, nolab);
        break;
    case MidCode::Instr::RET:
        if (midcode.t1() != nullptr) {
            t1 = REQ;
            GEN(move, Reg::v0, t1, noreg, noimm, nolab);
        }
        _regpool.clear();
        GEN(addi, Reg::sp, Reg::sp, noreg, _stackframe.size(), nolab);
        GEN(jr, Reg::ra, noreg, noreg, noimm, nolab);
        return;
    case MidCode::Instr::INPUT:
        GEN(li, Reg::v0, noreg, noreg, midcode.t0()->isInt() ? 5 : 12, nolab);
        GEN(syscall, noreg, noreg, noreg, noimm, nolab);
        t0 = REQ;
        GEN(move, t0, Reg::v0, noreg, noimm, nolab);
        break;
    case MidCode::Instr::OUTPUT_STR:
        GEN(move, Reg::t8, Reg::a0, noreg, noimm, nolab);
        GEN(la, Reg::a0, noreg, noreg, noimm,  strpool[midcode.labelName()]);
        GEN(li, Reg::v0, noreg, noreg, 4, nolab);
        GEN(syscall, noreg, noreg, noreg, noimm, nolab);
        GEN(move, Reg::a0, Reg::t8, noreg, noimm, nolab);
        break;
    case MidCode::Instr::OUTPUT_SYM:
        t1 = REQ;
        GEN(move, Reg::a0, t1, noreg, noimm, nolab);
        GEN(li, Reg::v0, noreg, noreg, midcode.t1()->isInt() ? 1 : 11, nolab);
        GEN(syscall, noreg, noreg, noreg, noimm, nolab);
        GEN(move, Reg::a0, Reg::t8, noreg, noimm, nolab);
        break;
    case MidCode::Instr::BGT:
    case MidCode::Instr::BGE:
    case MidCode::Instr::BLT:
    case MidCode::Instr::BLE:
    case MidCode::Instr::BEQ:
    case MidCode::Instr::BNE:
        t1 = _regpool.request();
        t2 = _regpool.request();
        _regpool.clear();
        _output(instrMap.at(midcode.instr()), noreg, t1, t2, noimm, midcode.labelName());
        return;
    case MidCode::Instr::GOTO:
        _regpool.clear();
        GEN(j, noreg, noreg, noreg, noimm, midcode.labelName());
        return;
    case MidCode::Instr::LABEL:
        GEN(label, noreg, noreg, noreg, noimm, midcode.labelName());
        break;
    default: assert(0);
    }
}

void Translator::_compileCallBlock(const BasicBlock& basicblock) {
    assert(basicblock.isFuncCall());
    const StackFrame& _stackframe = _regpool.stackframe();
    Reg t0, t1;
    
    for (Reg a : reg::a) {
        _stackframe.store(a);
    }
    _stackframe.store(Reg::ra);
    
    int argNum = basicblock.midcodes().size() - 1;
    for (int i = 0; i < argNum && i < reg::a.size(); i++) {
        t1 = REQ;
        if (std::find(reg::a.begin(), reg::a.end(), t1) == reg::a.end()) {
            GEN(move, reg::a[i], t1, noreg, noimm, nolab);
        } else {
            GEN(lw, reg::a[i], Reg::sp, noreg, _stackframe[t1], nolab);
        }
    }
    for (int i = reg::a.size(); i < argNum; i++) {
        t1 = REQ;
        if (std::find(reg::a.begin(), reg::a.end(), t1) == reg::a.end()) {
            GEN(sw, t1, Reg::sp, noreg, (i - argNum) * 4, nolab);
        } else {
            GEN(lw, Reg::t8, Reg::sp, noreg, _stackframe[t1], nolab);
            GEN(sw, Reg::t8, Reg::sp, noreg, (i - argNum) * 4, nolab);
        }
    }
    
    GEN(jal, noreg, noreg, noreg, noimm, basicblock.midcodes().back()->labelName());
    
    _stackframe.load(Reg::ra);
    for (Reg a : reg::a) {
        _stackframe.load(a);
    }
    
    if (basicblock.midcodes().back()->t0() != nullptr) {
        t0 = REQ;
        GEN(move, t0, Reg::v0, noreg, noimm, nolab);
    }
}
