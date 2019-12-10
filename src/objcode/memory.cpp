/**********************************************
    > File Name: memory.cpp
    > Author: Luting Wang
    > Mail: 2457348692@qq.com 
    > Created Time: Sun Nov 10 22:12:08 2019
 **********************************************/

#include <algorithm>
#include <cassert>
#include "symtable.h"

#include "./include/Reg.h"
#include "./include/ObjCode.h"

#include "./include/memory.h"

const Sbss* Sbss::_global = nullptr;

const Sbss* Sbss::global(void) {
    return _global;
}

void Sbss::init(void) {
    assert(_global == nullptr);
    std::set<const symtable::Entry*> globalSyms;
    SymTable::getTable().global().syms(globalSyms);
    _global = new Sbss(globalSyms);
}

void Sbss::deinit(void) {
    delete _global;
    _global = nullptr;
}

int Sbss::size(void) const {
    return _size;
}

Sbss::Sbss(const std::set<const symtable::Entry*>& syms) {
    for (auto& entry : syms) {
        if (entry->isConst()) { continue; }
        if (global() == nullptr) { // `this` is `_global`
            assert(entry->isGlobal());
        } else if (entry->isGlobal()) { // `entry` included in `_global`
            assert(_global->_syms.count(entry));
            continue;
        }
        _syms[entry] = _size;
        if (!entry->isArray()) { _size += WORD_SIZE; }
        else { _size += entry->value() * WORD_SIZE; }
    }
}

int Sbss::locate(const symtable::Entry* entry) const {
    return _syms.at(entry);
}

// regs stored in stackframe
const std::vector<Reg> regs = {
    Reg::s0, Reg::s1, Reg::s2, Reg::s3, Reg::s4, Reg::s5, Reg::s6, Reg::s7,
    Reg::t0, Reg::t1, Reg::t2, Reg::t3, Reg::t4, Reg::t5, Reg::t6, Reg::t7,
    Reg::ra,
    Reg::a0, Reg::a1, Reg::a2, Reg::a3
};

StackFrame::StackFrame(const CodeGen& output, std::vector<const symtable::Entry*> argList,
		const std::set<const symtable::Entry*>& syms) : Sbss(syms), _output(output) {
    // args should be eliminated from `syms` before hand
	for (auto& entry : syms) {
        assert(std::find(argList.begin(), argList.end(), entry) == argList.end());
	}

    // preserve space for regs
	_regBase = Sbss::size();
	_size = _regBase + regs.size() * WORD_SIZE;

    // parameters that are not passed through `reg::a`
	for (int i = reg::a.size(); i < argList.size(); i++) {
		_args[argList[i]] = _size;
		_size += WORD_SIZE;
	}
}

int StackFrame::size(void) const {
	return _size;
}

int StackFrame::locate(const symtable::Entry* const entry) const {
    assert(!entry->isGlobal());
	if (_args.count(entry)) { return _args.at(entry); }
	return Sbss::locate(entry);
}

int StackFrame::locateGlobal(const symtable::Entry* const entry) const {
    assert(entry->isGlobal());
    return global()->locate(entry);
}

int StackFrame::locate(const Reg reg) const {
	int offset = std::find(regs.begin(), regs.end(), reg) - regs.begin();
	assert(offset < regs.size()); // reg to be located need to be legal
	offset = _regBase + offset * WORD_SIZE;
	assert(offset < _size);
	return offset;
}

void StackFrame::storeReg(const Reg reg) const {
    _output(new Sw(reg, Reg::sp, locate(reg)));
}

void StackFrame::loadReg(const Reg reg) const {
    _output(new Lw(reg, Reg::sp, locate(reg)));
}

void StackFrame::storeSym(const Reg reg, const symtable::Entry* const entry) const {
    assert(!entry->isConst() && !entry->isArray());
    if (entry->isGlobal()) {
        _output(new Sw(reg, Reg::gp, global()->locate(entry)));
    } else {
        _output(new Sw(reg, Reg::sp, locate(entry)));
    }
}

void StackFrame::loadSym(const Reg reg, const symtable::Entry* const entry) const {
	assert(!entry->isConst() && !entry->isArray());
    if (entry->isGlobal()) {
        _output(new Lw(reg, Reg::gp, global()->locate(entry)));
    } else {
        _output(new Lw(reg, Reg::sp, locate(entry)));
    }
}
