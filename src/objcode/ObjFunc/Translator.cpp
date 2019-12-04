/**********************************************
    > File Name: Translator.cpp
    > Author: Luting Wang
    > Mail: 2457348692@qq.com 
    > Created Time: Tue Dec  3 18:48:16 2019
 **********************************************/

#include "./Translator.h"

Translator::Translator(CodeGen& output, RegPool& regpool) :
    _output(output), _regpool(regpool) {}
    
void Translator::_requiredSyms(std::vector<const symtable::Entry*>& _seq,
        std::vector<bool>& write,
        std::vector<bool>& mask,
        const MidCode& midcode) {
#define PUSH(t, w, m) \
        _seq.push_back(midcode.t()); \
        write.push_back(w); \
        mask.push_back(m)
    switch (midcode.instr()) {
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
    }
#undef PUSH
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
