/**********************************************
    > File Name: RegPool.cpp
    > Author: Luting Wang
    > Mail: 2457348692@qq.com 
    > Created Time: Sat Nov  9 00:00:58 2019
 **********************************************/

#include <algorithm>
#include <cassert>
#include "midcode/FlowChart.h"
#include "symtable.h"

#include "../include/memory.h"

#include "Action.h"
#include "Simulator.h"

#include "../include/RegPool.h"

RegPool::RegPool(const std::vector<const symtable::Entry*>& reg_a, const StackFrame& stackframe) :
	_reg_a(reg_a), _reg_s(reg::s.size(), nullptr),  _stackframe(stackframe) {
	assert(_reg_a.size() == reg::a.size());
}

void RegPool::genPrologue(void) const {
    for (int i = 0; i < reg::s.size(); i++) {
        if (_reg_s[i] == nullptr) { continue; }
        _stackframe.storeReg(reg::s[i]);
        _stackframe.loadSym(reg::s[i], _reg_s[i]);
    }
}

void RegPool::genEpilogue(void) const {
    for (int i = 0; i < reg::s.size(); i++) {
        if (_reg_s[i] == nullptr) { continue; }
        _stackframe.loadReg(reg::s[i]);
    }
}

void RegPool::simulate(const std::vector<const symtable::Entry*>& _seq,
		const std::vector<bool>& write,
		const std::vector<bool>& mask) {
    assert(_seq.size() == write.size() && _seq.size() == mask.size());
    assert(_actionCache.empty());
    ActionGen output = [this](const Reg reg, const symtable::Entry* const load, const symtable::Entry* const store)
            { this->_actionCache.push(new Action(reg, load, store)); };
    Simulator simu(output, _reg_a, _reg_s, _seq);
    for (int i = 0; i < _seq.size(); i++) {
        simu.request(write[i], mask[i]);
    }
	simu.clear();
}

void RegPool::_execute(void) {
	const Action* const action = _actionCache.front();
    _actionCache.pop();
    
    // store first!
    if (action->store != nullptr) {
        _stackframe.storeSym(action->reg, action->store);
	}
    if (action->load != nullptr) {
        _stackframe.loadSym(action->reg, action->load);
	}
    delete action;
}

Reg RegPool::request(void) {
    const Reg result = _actionCache.front()->reg;
	_execute();
    return result;
}

void RegPool::clear(void) {
	while (!_actionCache.empty()) {
        assert(_actionCache.front()->load == nullptr);
        assert(_actionCache.front()->store != nullptr);
		_execute();
	}
}
