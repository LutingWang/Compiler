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

#include "../include/StrPool.h"

#include "./Translator.h"
using Instr = ObjCode::Instr;

Translator::Translator(CodeGen& output, RegPool& regpool, const StackFrame& stackframe) :
    _output(output), _regpool(regpool), _stackframe(stackframe) {}

using Pusher = std::function<void(const symtable::Entry* const,
        const bool write, const bool mask)>;

void requiredSyms(Pusher& push, const MidCode* const midcode) {
    // deal with abnormalies first
    if (midcode->is(MidCode::Instr::STORE_IND)) {
        push(midcode->t2(), false, false);
        push(midcode->t1(), false, false);
        return;
    }
    
    if (midcode->is(MidCode::Instr::LOAD_IND)) {
        push(midcode->t2(), false, false);
    } else {
        if (midcode->t1IsValid()) {
            push(midcode->t1(), false, false);
        }
        if (midcode->t2IsValid()) {
            push(midcode->t2(), false, true);
        }
    }
    
    if (midcode->t0IsValid() && !midcode->t0()->isArray()) {
        push(midcode->t0(), true, false);
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
        t1 = _regpool.request(false, false);
        t2 = _regpool.request(false, true);
        t0 = _regpool.request(true, false);
        _output(*instr, t0, t1, t2, noimm, nolab);
        break;
    case MidCode::Instr::DIV:
        t1 = _regpool.request(false, false);
        t2 = _regpool.request(false, true);
        _output(Instr::div, noreg, t1, t2, noimm, nolab);
        t0 = _regpool.request(true, false);
        _output(Instr::mflo, t0, noreg, noreg, noimm, nolab);
        break;
    case MidCode::Instr::LOAD_IND:
        t2 = _regpool.request(false, false);
        _output(Instr::sll, reg::compiler_tmp, t2, noreg, LOG_WORD_SIZE, nolab);
        t0 = _regpool.request(true, false);
        _stackframe.loadInd(t0, midcode.t1(), reg::compiler_tmp);
        break;
    case MidCode::Instr::STORE_IND:
        t2 = _regpool.request(false, false);
        _output(Instr::sll, reg::compiler_tmp, t2, noreg, LOG_WORD_SIZE, nolab);
        t1 = _regpool.request(false, false);
        _stackframe.storeInd(t1, midcode.t0(), reg::compiler_tmp);
        break;
    case MidCode::Instr::ASSIGN:
        t1 = _regpool.request(false, false);
        t0 = _regpool.request(true, false);
        _output(Instr::move, t0, t1, noreg, noimm, nolab);
        break;
    case MidCode::Instr::RET:
        if (midcode.t1() != nullptr) {
            t1 = _regpool.request(false, false);
            _output(Instr::move, Reg::v0, t1, noreg, noimm, nolab);
        }
        _regpool.clear();
        _regpool.genEpilogue();
        _output(Instr::addi, Reg::sp, Reg::sp, noreg, _stackframe.size(), nolab);
        _output(Instr::jr, Reg::ra, noreg, noreg, noimm, nolab);
        break;
    case MidCode::Instr::INPUT:
        SYSCALL(midcode.t0()->isInt() ? 5 : 12);
        t0 = _regpool.request(true, false);
        _output(Instr::move, t0, Reg::v0, noreg, noimm, nolab);
        break;
    case MidCode::Instr::OUTPUT_STR:
        _output(Instr::move, reg::compiler_tmp, Reg::a0, noreg, noimm, nolab);
        _output(Instr::la, Reg::a0, noreg, noreg, noimm,  strpool[midcode.labelName()]);
        SYSCALL(4);
        _output(Instr::move, Reg::a0, reg::compiler_tmp, noreg, noimm, nolab);
        break;
    case MidCode::Instr::OUTPUT_INT:
        _output(Instr::move, reg::compiler_tmp, Reg::a0, noreg, noimm, nolab);
        t1 = _regpool.request(false, false);
        _output(Instr::move, Reg::a0, t1, noreg, noimm, nolab);
        SYSCALL(1);
        _output(Instr::move, Reg::a0, reg::compiler_tmp, noreg, noimm, nolab);
        break;
    case MidCode::Instr::OUTPUT_CHAR:
        _output(Instr::move, reg::compiler_tmp, Reg::a0, noreg, noimm, nolab);
        t1 = _regpool.request(false, false);
        _output(Instr::move, Reg::a0, t1, noreg, noimm, nolab);
        SYSCALL(11);
        _output(Instr::move, Reg::a0, reg::compiler_tmp, noreg, noimm, nolab);
        break;
    case MidCode::Instr::BGT:
    case MidCode::Instr::BGE:
    case MidCode::Instr::BLT:
    case MidCode::Instr::BLE:
    case MidCode::Instr::BEQ:
    case MidCode::Instr::BNE:
        t1 = _regpool.request(false, false);
        t2 = _regpool.request(false, true);
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
    
    _regpool.stash();
    
    // copy the first 4 parameters to `reg::a`
    int argNum = basicblock.midcodes().size() - 1;
    for (int i = 0; i < argNum && i < reg::a.size(); i++) {
        t1 = _regpool.request(false, false);
        if (std::find(reg::a.begin(), reg::a.end(), t1) == reg::a.end()) {
            _output(Instr::move, reg::a[i], t1, noreg, noimm, nolab);
        } else {
            _output(Instr::lw, reg::a[i], Reg::sp, noreg, _stackframe.locate(t1), nolab);
        }
    }
    
    // store the rest parameters to stack
    for (int i = reg::a.size(); i < argNum; i++) {
        t1 = _regpool.request(false, false);
        if (std::find(reg::a.begin(), reg::a.end(), t1) == reg::a.end()) {
            _output(Instr::sw, t1, Reg::sp, noreg, (i - argNum) * WORD_SIZE, nolab);
        } else {
            _output(Instr::lw, reg::compiler_tmp, Reg::sp, noreg, _stackframe.locate(t1), nolab);
            _output(Instr::sw, reg::compiler_tmp, Reg::sp, noreg, (i - argNum) * WORD_SIZE, nolab);
        }
    }
    
    // jump to the func
    _output(Instr::jal, noreg, noreg, noreg, noimm, basicblock.midcodes().back()->labelName());
    
    _regpool.unstash();
    
    // retrieve retval
    if (basicblock.midcodes().back()->t0() != nullptr) {
        t0 = _regpool.request(true, false);
        _output(Instr::move, t0, Reg::v0, noreg, noimm, nolab);
    }
}

void Translator::compile(const BasicBlock& basicblock) {
    std::vector<const symtable::Entry*> _seq;
    Pusher pusher = [&](const symtable::Entry* const entry, const bool w, const bool m) {
        assert(!entry->isArray());
        _seq.push_back(entry);
    };
    
    for (auto& midcode : basicblock.midcodes()) {
        requiredSyms(pusher, midcode);
    }
    
    _regpool.foresee(_seq);
    
    if (basicblock.isFuncCall()) {
        _compileCallBlock(basicblock);
    } else for (auto midcode : basicblock.midcodes()) {
        _compileCode(*midcode);
    }
    _regpool.clear();
}
