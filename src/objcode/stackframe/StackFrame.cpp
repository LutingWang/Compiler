/**********************************************
    > File Name: StackFrame.cpp
    > Author: Luting Wang
    > Mail: 2457348692@qq.com 
    > Created Time: Sun Nov 10 22:12:08 2019
 **********************************************/

#include <algorithm>
#include <cassert>
#include "symtable/SymTable.h"

#include "../include/Reg.h"
#include "../include/ObjCode.h"

#include "../include/memory.h"

namespace {
    auto& noreg = ObjCode::noreg;
    auto& noimm = ObjCode::noimm;
    auto& nolab = ObjCode::nolab;

    // regs stored in stackframe
    const std::vector<Reg> regs = {
        Reg::s0, Reg::s1, Reg::s2, Reg::s3, Reg::s4, Reg::s5, Reg::s6, Reg::s7,
        Reg::t0, Reg::t1, Reg::t2, Reg::t3, Reg::t4, Reg::t5, Reg::t6, Reg::t7,
        Reg::ra,
        Reg::a0, Reg::a1, Reg::a2, Reg::a3
    };
}

StackFrame::StackFrame(CodeGen& output, std::vector<const symtable::Entry*> argList,
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

int StackFrame::locate(const Reg reg) const {
	int offset = std::find(regs.begin(), regs.end(), reg) - regs.begin();
	assert(offset < regs.size()); // reg to be located need to be legal
	offset = _regBase + offset * WORD_SIZE;
	assert(offset < _size);
	return offset;
}
           
void StackFrame::_visit(const ObjCode::Instr instr, const Reg reg) const {
    assert(instr == ObjCode::Instr::sw || instr == ObjCode::Instr::lw);
    _output(instr, reg, Reg::sp, noreg, locate(reg), nolab);
}

void StackFrame::_visit(ObjCode::Instr instr, const Reg reg, const symtable::Entry* const entry) const {
    assert(instr == ObjCode::Instr::sw || instr == ObjCode::Instr::lw);
    assert(entry != nullptr && !entry->isArray());
    Reg t1;
    int imm;
    if (entry->isConst()) {
        assert(instr == ObjCode::Instr::lw);
        instr = ObjCode::Instr::li;
        t1 = noreg;
        imm = entry->value();
    } else if (entry->isGlobal()) {
        t1 = Reg::gp;
        imm = global()->locate(entry);
    } else {
        t1 = Reg::sp;
        imm = locate(entry);
    }
    _output(instr, reg, t1, noreg, imm, nolab);
}

void StackFrame::_visit(const ObjCode::Instr instr, const Reg reg,
        const symtable::Entry* const entry, const Reg ind) const {
    assert(instr == ObjCode::Instr::sw || instr == ObjCode::Instr::lw);
    assert(entry != nullptr && entry->isArray()); // implies non-const
    Reg base;
    int imm;
    if (entry->isGlobal()) {
        base = Reg::gp;
        imm = global()->locate(entry);
    } else {
        base = Reg::sp;
        imm = locate(entry);
    }
    _output(ObjCode::Instr::add, reg::stackframe_tmp, ind, base, noimm, nolab);
    _output(instr, reg, reg::stackframe_tmp, noreg, imm, nolab);
}

void StackFrame::storeReg(const Reg reg) const {
    _visit(ObjCode::Instr::sw, reg);
}

void StackFrame::loadReg(const Reg reg) const {
   _visit(ObjCode::Instr::lw, reg);
}

void StackFrame::storeSym(const Reg reg, const symtable::Entry* const entry) const {
	_visit(ObjCode::Instr::sw, reg, entry);
}

void StackFrame::loadSym(const Reg reg, const symtable::Entry* const entry) const {
	_visit(ObjCode::Instr::lw, reg, entry);
}

void StackFrame::storeInd(const Reg reg, const symtable::Entry* const entry, const Reg ind) const {
    _visit(ObjCode::Instr::sw, reg, entry, ind);
}

void StackFrame::loadInd(const Reg reg, const symtable::Entry* const entry, const Reg ind) const {
    _visit(ObjCode::Instr::lw, reg, entry, ind);
}
