/**********************************************
    > File Name: Translator.cpp
    > Author: Luting Wang
    > Mail: 2457348692@qq.com 
    > Created Time: Tue Dec  3 18:48:16 2019
 **********************************************/

#include <cassert>
#include "datastream/LiveVar.h"

#include "./Translator.h"

Translator::Translator(CodeGen& output, RegPool& regpool, const StackFrame& stackframe) :
    _output(output), _regpool(regpool), _stackframe(stackframe) {}
    
void Translator::_requiredSyms(std::vector<const symtable::Entry*>& _seq,
        std::vector<bool>& write,
        std::vector<bool>& mask,
        const MidCode* const midcode) {
#define PUSH(t, w, m) _seq.push_back(t); \
    write.push_back(w); mask.push_back(m)
    
    std::vector<const symtable::Entry*> use;
    LiveVar::use(use, midcode);
    if (midcode->is(MidCode::Instr::STORE_IND)) {
        assert(use.size() == 2);
        PUSH(use[1], false, false);
        PUSH(use[0], false, false);
    } else if (use.size() == 2) {
        PUSH(use[0], false, false);
        PUSH(use[1], false, true);
    } else if (use.size() == 1) {
        PUSH(use[0], false, false);
    }
    
    const symtable::Entry* def = nullptr;
    LiveVar::def(def, midcode);
    if (def != nullptr) {
        PUSH(def, true, false);
    }
#undef PUSH
}

void Translator::compile(const BasicBlock& basicblock) {
    std::vector<const symtable::Entry*> _seq;
    std::vector<bool> write;
    std::vector<bool> mask;
    for (auto& midcode : basicblock.midcodes()) {
        _requiredSyms(_seq, write, mask, midcode);
    }
    _regpool.simulate(_seq, write, mask);
    
    if (basicblock.isFuncCall()) {
        _compileCallBlock(basicblock);
    } else for (auto midcode : basicblock.midcodes()) {
        _compileCode(*midcode);
    }
    _regpool.clear();
}
