/**********************************************
    > File Name: Translator.cpp
    > Author: Luting Wang
    > Mail: 2457348692@qq.com 
    > Created Time: Tue Dec  3 18:48:16 2019
 **********************************************/

#include <algorithm>
#include <cassert>
#include <functional>
#include <map>
#include "datastream/LiveVar.h"

#include "../include/StrPool.h"

#include "./Translator.h"
using Instr = ObjCode::Instr;

Translator::Translator(CodeGen& output, RegPool& regpool, const StackFrame& stackframe) :
    _output(output), _regpool(regpool), _stackframe(stackframe) {}

using Pusher = std::function<void(const symtable::Entry* const, const bool, const bool)>;

void requiredSyms(Pusher& push, const MidCode* const midcode) {
    std::vector<const symtable::Entry*> use;
    LiveVar::use(use, midcode);
    if (midcode->is(MidCode::Instr::STORE_IND)) {
        assert(use.size() == 2);
        push(use[1], false, false);
        push(use[0], false, false);
    } else if (use.size() == 2) {
        push(use[0], false, false);
        push(use[1], false, true);
    } else if (use.size() == 1) {
        push(use[0], false, false);
    }
    
    const symtable::Entry* def = nullptr;
    LiveVar::def(def, midcode);
    if (def != nullptr) {
        push(def, true, false);
    }
}

namespace {
    auto& noreg =  ObjCode::noreg;
    auto& noimm =  ObjCode::noimm;
    auto& nolab =  ObjCode::nolab;

    const std::map<MidCode::Instr, Instr> instrMap = {
        { MidCode::Instr::ADD, Instr::add },
        { MidCode::Instr::SUB, Instr::sub },
        { MidCode::Instr::MULT, Instr::mul },
        { MidCode::Instr::BGT, Instr::bgt },
        { MidCode::Instr::BGE, Instr::bge },
        { MidCode::Instr::BLT, Instr::blt },
        { MidCode::Instr::BLE, Instr::ble },
        { MidCode::Instr::BEQ, Instr::beq },
        { MidCode::Instr::BNE, Instr::bne },
        { MidCode::Instr::GOTO, Instr::j },
        { MidCode::Instr::LABEL, Instr::label }
    };
}

#define REQ _regpool.request()

#define SYSCALL(id) \
    _output(Instr::li, Reg::v0, noreg, noreg, (id), nolab); \
    _output(Instr::syscall, noreg, noreg, noreg, noimm, nolab);

// NOTE: clear regpool right before ret, branch, or jump
void Translator::_compileCode(const MidCode& midcode) {
    const Instr* instr = nullptr;
    if (instrMap.count(midcode.instr())) {
        instr = &instrMap.at(midcode.instr());
    }
    Reg t0, t1, t2;
    
    switch (midcode.instr()) {
    case MidCode::Instr::ADD:
    case MidCode::Instr::SUB:
    case MidCode::Instr::MULT:
        t1 = REQ;
        t2 = REQ;
        t0 = REQ;
        _output(*instr, t0, t1, t2, noimm, nolab);
        break;
    case MidCode::Instr::DIV:
        t1 = REQ;
        t2 = REQ;
        _output(Instr::div, noreg, t1, t2, noimm, nolab);
        t0 = REQ;
        _output(Instr::mflo, t0, noreg, noreg, noimm, nolab);
        break;
    case MidCode::Instr::LOAD_IND:
        t2 = REQ;
        _output(Instr::sll, reg::compiler_tmp, t2, noreg, LOG_WORD_SIZE, nolab);
        t0 = REQ;
        _stackframe.loadInd(t0, midcode.t1(), reg::compiler_tmp);
        break;
    case MidCode::Instr::STORE_IND:
        t2 = REQ;
        _output(Instr::sll, reg::compiler_tmp, t2, noreg, LOG_WORD_SIZE, nolab);
        t1 = REQ;
        _stackframe.storeInd(t1, midcode.t0(), reg::compiler_tmp);
        break;
    case MidCode::Instr::ASSIGN:
        t1 = REQ;
        t0 = REQ;
        _output(Instr::move, t0, t1, noreg, noimm, nolab);
        break;
    case MidCode::Instr::RET:
        if (midcode.t1() != nullptr) {
            t1 = REQ;
            _output(Instr::move, Reg::v0, t1, noreg, noimm, nolab);
        }
        _regpool.clear();
        _regpool.genEpilogue();
        _output(Instr::addi, Reg::sp, Reg::sp, noreg, _stackframe.size(), nolab);
        _output(Instr::jr, Reg::ra, noreg, noreg, noimm, nolab);
        break;
    case MidCode::Instr::INPUT:
        SYSCALL(midcode.t0()->isInt() ? 5 : 12);
        t0 = REQ;
        _output(Instr::move, t0, Reg::v0, noreg, noimm, nolab);
        break;
    case MidCode::Instr::OUTPUT_STR:
        _output(Instr::move, reg::compiler_tmp, Reg::a0, noreg, noimm, nolab);
        _output(Instr::la, Reg::a0, noreg, noreg, noimm,  strpool[midcode.labelName()]);
        SYSCALL(4);
        _output(Instr::move, Reg::a0, reg::compiler_tmp, noreg, noimm, nolab);
        break;
    case MidCode::Instr::OUTPUT_SYM:
        _output(Instr::move, reg::compiler_tmp, Reg::a0, noreg, noimm, nolab);
        t1 = REQ;
        _output(Instr::move, Reg::a0, t1, noreg, noimm, nolab);
        SYSCALL(midcode.t1()->isInt() ? 1 : 11);
        _output(Instr::move, Reg::a0, reg::compiler_tmp, noreg, noimm, nolab);
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
        _output(*instr, noreg, t1, t2, noimm, midcode.labelName());
        break;
    case MidCode::Instr::GOTO:
        _regpool.clear();
        _output(*instr, noreg, noreg, noreg, noimm, midcode.labelName());
        break;
    case MidCode::Instr::LABEL:
        _output(*instr, noreg, noreg, noreg, noimm, midcode.labelName());
        break;
    default: assert(0);
    }
}

void Translator::_compileCallBlock(const BasicBlock& basicblock) {
    assert(basicblock.isFuncCall());
    Reg t0, t1;
    
    for (Reg a : reg::a) { _stackframe.storeReg(a); } // backup `reg::a`
    _stackframe.storeReg(Reg::ra); // backup `Reg::ra`
    
    // copy the first 4 parameters to `reg::a`
    int argNum = basicblock.midcodes().size() - 1;
    for (int i = 0; i < argNum && i < reg::a.size(); i++) {
        t1 = REQ;
        if (std::find(reg::a.begin(), reg::a.end(), t1) == reg::a.end()) {
            _output(Instr::move, reg::a[i], t1, noreg, noimm, nolab);
        } else {
            _output(Instr::lw, reg::a[i], Reg::sp, noreg, _stackframe.locate(t1), nolab);
        }
    }
    
    // store the rest parameters to stack
    for (int i = reg::a.size(); i < argNum; i++) {
        t1 = REQ;
        if (std::find(reg::a.begin(), reg::a.end(), t1) == reg::a.end()) {
            _output(Instr::sw, t1, Reg::sp, noreg, (i - argNum) * WORD_SIZE, nolab);
        } else {
            _output(Instr::lw, reg::compiler_tmp, Reg::sp, noreg, _stackframe.locate(t1), nolab);
            _output(Instr::sw, reg::compiler_tmp, Reg::sp, noreg, (i - argNum) * WORD_SIZE, nolab);
        }
    }
    
    // jump to the func
    _output(Instr::jal, noreg, noreg, noreg, noimm, basicblock.midcodes().back()->labelName());
    
    _stackframe.loadReg(Reg::ra); // restore `Reg::ra`
    for (Reg a : reg::a) { _stackframe.loadReg(a); } // restore `reg::a`
    
    // retrieve retval
    if (basicblock.midcodes().back()->t0() != nullptr) {
        t0 = REQ;
        _output(Instr::move, t0, Reg::v0, noreg, noimm, nolab);
    }
}

void Translator::compile(const BasicBlock& basicblock) {
    std::vector<const symtable::Entry*> _seq;
    std::vector<bool> write;
    std::vector<bool> mask;
    Pusher pusher = [&](const symtable::Entry* const entry, const bool w, const bool m) {
        _seq.push_back(entry); write.push_back(w); mask.push_back(m);
    };
    for (auto& midcode : basicblock.midcodes()) {
        requiredSyms(pusher, midcode);
    }
    
    _regpool.simulate(_seq, write, mask);
    
    if (basicblock.isFuncCall()) {
        _compileCallBlock(basicblock);
    } else for (auto midcode : basicblock.midcodes()) {
        _compileCode(*midcode);
    }
    _regpool.clear();
}
