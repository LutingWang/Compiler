/**********************************************
    > File Name: Translator.cpp
    > Author: Luting Wang
    > Mail: 2457348692@qq.com 
    > Created Time: Tue Dec  3 18:48:16 2019
 **********************************************/

#include <cassert>
#include <functional>
#include "datastream/LiveVar.h"

#include "./Translator.h"

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
