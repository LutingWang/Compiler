/**********************************************
    > File Name: StackFrame.cpp
    > Author: Luting Wang
    > Mail: 2457348692@qq.com 
    > Created Time: Sun Nov 10 22:12:08 2019
 **********************************************/

#include <algorithm>
#include <cassert>
#include <map>
#include <set>
#include <vector>
#include "symtable.h"
#include "Mips.h"

#include "../include/Reg.h"

#include "../include/ObjCode.h"
#include "../include/memory.h"

StackFrame::StackFrame(std::vector<ObjCode>& output, 
		std::vector<symtable::Entry*> argList, 
		std::set<symtable::Entry*> syms) :
	Sbss(syms), _output(output) {

	for (auto& entry : syms) {
        assert(std::find(argList.begin(), argList.end(), entry) == argList.end());
	}

	_regBase = _size;
	_size += (reg::s.size() + reg::t.size() + 1 /* ra */ + reg::a.size()) * 4;

	for (int i = 4; i < argList.size(); i++) {
		_args[argList[i]] = _size;
		_size += 4;
	}
}

int StackFrame::size(void) const {
	return _size;
}

int StackFrame::operator [] (symtable::Entry* const entry) const {
	if (_args.count(entry)) {
		return _args.at(entry);
	}
	if (contains(entry)) {
		return _locate(entry);
	} else {
		return Sbss::global()->_locate(entry);
	}
}

int StackFrame::operator [] (Reg reg) const {
	static const std::vector<Reg> regs = {
		Reg::s0, Reg::s1, Reg::s2, Reg::s3, Reg::s4, Reg::s5, Reg::s6, Reg::s7,
		Reg::t0, Reg::t1, Reg::t2, Reg::t3, Reg::t4, Reg::t5, Reg::t6, Reg::t7,
		Reg::ra,
		Reg::a0, Reg::a1, Reg::a2, Reg::a3
	};

	int offset = std::find(regs.begin(), regs.end(), reg) - regs.begin();
	assert(offset < regs.size());
	offset = _regBase + offset * 4;
	assert(offset < _size);
	return offset;
}

void StackFrame::_visit(bool isLoad, Reg reg, symtable::Entry* const entry) {
	if (entry == nullptr) {
		_output.emplace_back(isLoad ? ObjCode::Instr::lw : ObjCode::Instr::sw, 
				reg, Reg::sp, Reg::zero, operator[](reg), "");
		return;
	}
	if (entry->isConst) {
		_output.emplace_back(ObjCode::Instr::li, reg, Reg::zero, Reg::zero, entry->value, "");
		return;
	}
	if (_args.count(entry)) {
		_output.emplace_back(ObjCode::Instr::lw, reg, Reg::sp, Reg::zero, _args[entry], "");
		return;
	}
	if (contains(entry)) {
		_output.emplace_back(isLoad ? ObjCode::Instr::lw : ObjCode::Instr::sw, 
				reg, Reg::sp, Reg::zero, _locate(entry), "");
	} else {
		_output.emplace_back(isLoad ? ObjCode::Instr::lw : ObjCode::Instr::sw, 
				reg, Reg::gp, Reg::zero, Sbss::global()->_locate(entry), "");
	}
}

void StackFrame::store(Reg reg) {
	_visit(false, reg);
}

void StackFrame::store(Reg reg, symtable::Entry* const entry) {
	_visit(false, reg, entry);
}

void StackFrame::load(Reg reg) {
	_visit(true, reg);
}

void StackFrame::load(Reg reg, symtable::Entry* const entry) {
	_visit(true, reg, entry);
}
