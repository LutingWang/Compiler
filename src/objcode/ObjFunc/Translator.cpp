/**********************************************
    > File Name: Translator.cpp
    > Author: Luting Wang
    > Mail: 2457348692@qq.com 
    > Created Time: Tue Dec  3 18:48:16 2019
 **********************************************/

#include "../include/memory.h"
#include "../include/StrPool.h"

#include "./Translator.h"

namespace {
    const Reg noreg = Reg::zero;
    const int noimm = 0;
    const std::string nolab = "";
}

Translator::Translator(std::vector<const ObjCode*>& output, RegPool& regpool) : _output(output), _regpool(regpool) {}
    
void Translator::_requiredSyms(std::vector<const symtable::Entry*>& _seq,
        std::vector<bool>& write,
        std::vector<bool>& mask,
        const MidCode& midcode) {
    switch (midcode.instr()) {
#define PUSH(t, w, m) \
        _seq.push_back(midcode.t()); \
        write.push_back(w); \
        mask.push_back(m)

    case MidCode::Instr::ADD:
    case MidCode::Instr::SUB:
    case MidCode::Instr::MULT:
    case MidCode::Instr::DIV:
        PUSH(t1, false, false);
        PUSH(t2, false, true);
        PUSH(t0, true, false);
        break;
    case MidCode::Instr::LOAD_IND:
        PUSH(t2, false, false);
        PUSH(t0, true, false);
        break;
    case MidCode::Instr::STORE_IND:
        PUSH(t2, false, false);
        PUSH(t1, false, false);
        break;
    case MidCode::Instr::ASSIGN:
        PUSH(t1, false, false);
        PUSH(t0, true, false);
        break;
    case MidCode::Instr::PUSH_ARG:
        PUSH(t1, false, false);
        break;
    case MidCode::Instr::CALL:
        if (midcode.t0() != nullptr) { PUSH(t0, true, false); }
        break;
    case MidCode::Instr::RET:
        if (midcode.t1() != nullptr) { PUSH(t1, false, false); }
        break;
    case MidCode::Instr::INPUT:
        PUSH(t0, true, false);
        break;
    case MidCode::Instr::OUTPUT_SYM:
        PUSH(t1, false, false);
        break;
    case MidCode::Instr::BGT:
    case MidCode::Instr::BGE:
    case MidCode::Instr::BLT:
    case MidCode::Instr::BLE:
    case MidCode::Instr::BEQ:
    case MidCode::Instr::BNE:
        PUSH(t1, false, false);
        PUSH(t2, false, true);
        break;
    default: break;
#undef PUSH
    }
}

void Translator::requiredSyms(std::vector<const symtable::Entry*>& _seq,
        const symtable::FuncTable* const functable) {
    assert(0);
}

void Translator::_compileCode(const MidCode& midcode) {
    const StackFrame& _stackframe = _regpool.stackframe();
    Reg t0, t1, t2;
    switch (midcode.instr()) {
#define GEN(id, t0, t1, t2, imm, lab) \
    _output.push_back(new ObjCode(ObjCode::Instr::id, t0, t1, t2, imm, lab));
#define REQ _regpool.request()
    case MidCode::Instr::ADD:
        t1 = REQ;
        t2 = REQ;
        t0 = REQ;
        GEN(add, t0, t1, t2, noimm, nolab);
        break;
    case MidCode::Instr::SUB:
        t1 = REQ;
        t2 = REQ;
        t0 = REQ;
        GEN(sub, t0, t1, t2, noimm, nolab);
        break;
    case MidCode::Instr::MULT:
        t1 = REQ;
        t2 = REQ;
        t0 = REQ;
        GEN(mul, t0, t1, t2, noimm, nolab);
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
    case MidCode::Instr::PUSH_ARG:
    case MidCode::Instr::CALL:
        assert(0);
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
            
#define CASE(id, op) case MidCode::Instr::id: \
        t1 = REQ; t2 = REQ; \
        _regpool.clear(); \
        GEN(op, noreg, t1, t2, noimm, midcode.labelName()); \
        return
            
    CASE(BGT, bgt); CASE(BGE, bge); CASE(BLT, blt); CASE(BLE, ble);
    CASE(BEQ, beq); CASE(BNE, bne);
#undef CASE

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
        _output.push_back(_stackframe.store(a));
    }
    _output.push_back(_stackframe.store(Reg::ra));
    
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
    
    _output.push_back(_stackframe.load(Reg::ra));
    for (Reg a : reg::a) {
        _output.push_back(_stackframe.load(a));
    }
    
    if (basicblock.midcodes().back()->t0() != nullptr) {
        t0 = REQ;
        GEN(move, t0, Reg::v0, noreg, noimm, nolab);
    }
}

void Translator::compile(const BasicBlock& basicblock) {
    std::vector<const symtable::Entry*> _seq;
    std::vector<bool> write;
    std::vector<bool> mask;
    for (auto& midcode : basicblock.midcodes()) {
        _requiredSyms(_seq, write, mask, *midcode);
    }
    _regpool.simulate(_seq, write, mask);
    if (basicblock.isFuncCall()) {
        _compileCallBlock(basicblock);
    } else for (auto midcode : basicblock.midcodes()) {
        _compileCode(*midcode);
    }
    _regpool.clear();
}
