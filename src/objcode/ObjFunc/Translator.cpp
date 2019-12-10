/**********************************************
    > File Name: Translator.cpp
    > Author: Luting Wang
    > Mail: 2457348692@qq.com 
    > Created Time: Tue Dec  3 18:48:16 2019
 **********************************************/

#include <algorithm>
#include <cassert>
#include <map>

#include "../include/ObjCode.h"
#include "../include/StrPool.h"

#include "./Translator.h"
using Instr = MidCode::Instr;

/* UsageQueue */

// The usage sequence of variables.
class UsageQueue {
public:
    class Usage {
        friend class UsageQueue;
        bool _useReg;
        Reg _reg = Reg::zero;
        int _imm;
    public:
        bool useReg(void) const;
        Reg getReg(void) const;
        int getImm(void) const;
        
        Usage(void);
        Usage(const int);
    };
    
private:
    RegPool& _regpool;
    std::vector<Usage> _queue;
    
    void _emplace_back(const symtable::Entry* const);
public:
    UsageQueue(const BasicBlock* const, RegPool&);
    
    const Usage pop(const bool write, const bool mask);
};

bool UsageQueue::Usage::useReg(void) const {
    return _useReg;
}

Reg UsageQueue::Usage::getReg(void) const {
    assert(useReg() && _imm == 0);
    return _reg;
}

int UsageQueue::Usage::getImm(void) const {
    assert(!useReg() && _reg == Reg::zero);
    return _imm;
}

UsageQueue::Usage::Usage(void) : _useReg(true), _imm(0) {}
UsageQueue::Usage::Usage(const int imm) : _useReg(false), _imm(imm) {}

void UsageQueue::_emplace_back(const symtable::Entry* const entry) {
    if (entry->isConst()) { _queue.emplace_back(entry->value()); }
    else {
        _queue.emplace_back();
        _regpool.foresee(entry);
    }
}

UsageQueue::UsageQueue(const BasicBlock* const basicblock, RegPool& regpool) :
    _regpool(regpool) {
    for (auto midcode : basicblock->midcodes()) {
        if (midcode->is(Instr::STORE_IND)) {
            _emplace_back(midcode->t2());
            _emplace_back(midcode->t1());
            continue;
        }
        
        if (midcode->t1IsValid() && !midcode->t1()->isArray()) {
            _emplace_back(midcode->t1());
        }
        if (midcode->t2IsValid() && !midcode->t2()->isArray()) {
            _emplace_back(midcode->t2());
        }
        if (midcode->t0IsValid() && !midcode->t0()->isArray()) {
            _emplace_back(midcode->t0());
        }
    }
}

const UsageQueue::Usage UsageQueue::pop(const bool write, const bool mask) {
    Usage usage = _queue[0];
    _queue.erase(_queue.begin());
    
    if (usage.useReg()) {
        usage._reg = _regpool.request(write, mask);
    } else if (usage.getImm() == 0) {
        // for const 0 request reg::zero
        usage._useReg = true;
    }
    return usage;
}

/* Translator */

Translator::Translator(const objcode::CodeGen& output, RegPool& regpool, const StackFrame& stackframe) :
    _output(output), _regpool(regpool), _stackframe(stackframe) {}

template<typename factory>
void Translator::_compileArith(UsageQueue& usagequeue) {
    auto t1 = usagequeue.pop(false, false);
    auto t2 = usagequeue.pop(false, t1.useReg());
    auto t0 = usagequeue.pop(true, false);
    assert(t0.useReg());
    if (t1.useReg()) {
        if (t2.useReg()) { _output(factory::produce(t0.getReg(), t1.getReg(), t2.getReg())); }
        else { _output(factory::produce(t0.getReg(), t1.getReg(), t2.getImm())); }
    } else {
        if (t2.useReg()) { _output(factory::produce(t0.getReg(), t1.getImm(), t2.getReg())); }
        else { _output(factory::produce(t0.getReg(), t1.getImm(), t2.getImm())); }
    }
}

template<typename factory>
void Translator::_compileBranch(UsageQueue& usagequeue, const std::string& label) {
    auto t1 = usagequeue.pop(false, false);
    auto t2 = usagequeue.pop(false, t1.useReg());
    _regpool.clear();
    if (t1.useReg()) {
        if (t2.useReg()) { _output(factory::produce(t1.getReg(), t2.getReg(), label)); }
        else { _output(factory::produce(t1.getReg(), t2.getImm(), label)); }
    } else {
        if (t2.useReg()) { _output(factory::produce(t1.getImm(), t2.getReg(), label)); }
        else { _output(factory::produce(t1.getImm(), t2.getImm(), label)); }
    }
}

// NOTE: clear regpool right before ret, branch, or jump
void Translator::_compileCode(const MidCode* const midcode, UsageQueue& usagequeue) {
    switch (midcode->instr()) {
    case Instr::ADD: _compileArith<objcode::AddFactory>(usagequeue); break;
    case Instr::SUB: _compileArith<objcode::SubFactory>(usagequeue); break;
    case Instr::MULT: _compileArith<objcode::MulFactory>(usagequeue); break;
    case Instr::DIV: _compileArith<objcode::DivFactory>(usagequeue); break;
    case Instr::LOAD_IND: {
        int offset = midcode->t1()->isGlobal() ?
                _stackframe.locateGlobal(midcode->t1()) :
                _stackframe.locate(midcode->t1());
        const Reg base = midcode->t1()->isGlobal() ? Reg::gp : Reg::sp;
        
        auto t2 = usagequeue.pop(false, false);
        auto t0 = usagequeue.pop(true, false);
        if (t2.useReg()) {
            _output(new objcode::Sll(reg::compiler_tmp, t2.getReg(), LOG_WORD_SIZE));
            _output(new objcode::Add(reg::compiler_tmp, reg::compiler_tmp, base));
            _output(new objcode::Lw(t0.getReg(), reg::compiler_tmp, offset));
        } else {
            offset += t2.getImm() << LOG_WORD_SIZE;
            _output(new objcode::Lw(t0.getReg(), base, offset));
        }
        break;
    }
    case Instr::STORE_IND: {
        int offset = midcode->t0()->isGlobal() ?
                _stackframe.locateGlobal(midcode->t0()) :
                _stackframe.locate(midcode->t0());
        const Reg base = midcode->t0()->isGlobal() ? Reg::gp : Reg::sp;
        
        auto t2 = usagequeue.pop(false, false);
        if (t2.useReg()) { // store offset to t0 temporarily
            _output(new objcode::Sll(reg::compiler_tmp, t2.getReg(), LOG_WORD_SIZE));
        }
        
        auto t1 = usagequeue.pop(false, false);
        Reg data = reg::stackframe_tmp; // holding data to store
        if (t1.useReg()) { data = t1.getReg(); }
        else { _output(new objcode::Li(reg::stackframe_tmp, t1.getImm())); }
            
        if (t2.useReg()) {
            _output(new objcode::Add(reg::compiler_tmp, reg::compiler_tmp, base));
            _output(new objcode::Sw(data, reg::compiler_tmp, offset));
        } else {
            offset += t2.getImm() << LOG_WORD_SIZE;
            _output(new objcode::Sw(data, base, offset));
        }
        break;
    }
    case Instr::ASSIGN: {
        auto t1 = usagequeue.pop(false, false);
        auto t0 = usagequeue.pop(true, false);
        if (t1.useReg()) {
            _output(new objcode::Move(t0.getReg(), t1.getReg()));
        } else {
            _output(new objcode::Li(t0.getReg(), t1.getImm()));
        }
        break;
    }
    case Instr::RET:
        if (midcode->t1IsValid()) {
            auto t1 = usagequeue.pop(false, false);
            if (t1.useReg()) {
                _output(new objcode::Move(Reg::v0, t1.getReg()));
            } else {
                _output(new objcode::Li(Reg::v0, t1.getImm()));
            }
        }
        _regpool.clear();
        _regpool.genEpilogue();
            _output(objcode::AddFactory::produce(Reg::sp, Reg::sp, _stackframe.size()));
            _output(new objcode::Jr());
        break;
    case Instr::INPUT: {
        _output(new objcode::Li(Reg::v0, midcode->t0()->isInt() ? 5 : 12));
        _output(new objcode::Syscall());
        auto t0 = usagequeue.pop(true, false);
        _output(new objcode::Move(t0.getReg(), Reg::v0));
        break;
    }
    case Instr::OUTPUT_STR:
            _output(new objcode::Move(reg::compiler_tmp, Reg::a0));
            _output(new objcode::La(Reg::a0, strpool[midcode->labelName()]));
            _output(new objcode::Li(Reg::v0, 4));
            _output(new objcode::Syscall());
            _output(new objcode::Move(Reg::a0, reg::compiler_tmp));
        break;
    case Instr::OUTPUT_INT: case Instr::OUTPUT_CHAR: {
        _output(new objcode::Move(reg::compiler_tmp, Reg::a0));
        auto t1 = usagequeue.pop(false, false);
        if (t1.useReg()) { _output(new objcode::Move(Reg::a0, t1.getReg())); }
        else { _output(new objcode::Li(Reg::a0, t1.getImm())); }
        _output(new objcode::Li(Reg::v0, midcode->is(Instr::OUTPUT_INT) ? 1 : 11));
        _output(new objcode::Syscall());
        _output(new objcode::Move(Reg::a0, reg::compiler_tmp));
        break;
    }
    case Instr::BGT: _compileBranch<objcode::BgtFactory>(usagequeue, midcode->labelName()); break;
    case Instr::BGE: _compileBranch<objcode::BgeFactory>(usagequeue, midcode->labelName()); break;
    case Instr::BLT: _compileBranch<objcode::BltFactory>(usagequeue, midcode->labelName()); break;
    case Instr::BLE: _compileBranch<objcode::BleFactory>(usagequeue, midcode->labelName()); break;
    case Instr::BEQ: _compileBranch<objcode::BeqFactory>(usagequeue, midcode->labelName()); break;
    case Instr::BNE: _compileBranch<objcode::BneFactory>(usagequeue, midcode->labelName()); break;
    case MidCode::Instr::GOTO:
        _regpool.clear();
            _output(new objcode::J(midcode->labelName()));
        break;
    case MidCode::Instr::LABEL:
            _output(new objcode::Label(midcode->labelName()));
        break;
    default: assert(0);
    }
}

void Translator::_compileCallBlock(const BasicBlock* const basicblock, UsageQueue& usagequeue) {
    assert(basicblock->isFuncCall());
    
    _regpool.stash(); // save ra and s
    
    // copy the first 4 parameters to `reg::a`
    int argNum = basicblock->midcodes().size() - 1;
    for (int i = 0; i < argNum && i < reg::a.size(); i++) {
        auto t1 = usagequeue.pop(false, false);
        if (!t1.useReg()) {
            _output(new objcode::Li(reg::a[i], t1.getImm()));
        } else if (std::find(reg::a.begin(), reg::a.end(), t1.getReg()) == reg::a.end()) {
            _output(new objcode::Move(reg::a[i], t1.getReg()));
        } else {
            _output(new objcode::Lw(reg::a[i], Reg::sp, _stackframe.locate(t1.getReg())));
        }
    }
    
    // store the rest parameters to stack
    for (int i = reg::a.size(); i < argNum; i++) {
        const int offset = (i - argNum) * WORD_SIZE;
        auto t1 = usagequeue.pop(false, false);
        if (!t1.useReg()) {
            _output(new objcode::Li(reg::compiler_tmp, t1.getImm()));
            _output(new objcode::Sw(reg::compiler_tmp, Reg::sp, offset));
        } else if (std::find(reg::a.begin(), reg::a.end(), t1.getReg()) == reg::a.end()) {
            _output(new objcode::Sw(t1.getReg(), Reg::sp, offset));
        } else {
            _output(new objcode::Lw(reg::compiler_tmp, Reg::sp, _stackframe.locate(t1.getReg())));
            _output(new objcode::Sw(reg::compiler_tmp, Reg::sp, offset));
        }
    }
    
    _output(new objcode::Jal(basicblock->midcodes().back()->labelName())); // jump to the func
    _regpool.unstash(); // restore ra and s
    
    // retrieve retval
    if (basicblock->midcodes().back()->t0() != nullptr) {
        auto t0 = usagequeue.pop(true, false);
        _output(new objcode::Move(t0.getReg(), Reg::v0));
    }
}

void Translator::compile(const BasicBlock* const basicblock) {
    UsageQueue usagequeue(basicblock, _regpool);
    
    if (basicblock->isFuncCall()) {
        _compileCallBlock(basicblock, usagequeue);
    } else for (auto midcode : basicblock->midcodes()) {
        _compileCode(midcode, usagequeue);
    }
    _regpool.clear();
}
