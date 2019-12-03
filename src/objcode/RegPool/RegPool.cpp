/**********************************************
    > File Name: RegPool.cpp
    > Author: Luting Wang
    > Mail: 2457348692@qq.com 
    > Created Time: Sat Nov  9 00:00:58 2019
 **********************************************/

#include <cassert>
#include <set>
#include <vector>
#include "midcode/MidCode.h"
#include "symtable/SymTable.h"

#include "Action.h"
#include "Simulator.h"

#include "../include/RegPool.h"
#include "../include/memory.h"

RegPool::RegPool(const std::vector<const MidCode*>& midcode, 
		const std::vector<const symtable::Entry*>& reg_a) : 
	_reg_a(reg_a) {
	assert(_reg_a.size() == reg::a.size());

	// TODO: assign saved registers
	_reg_s = std::vector<const symtable::Entry*>(8, nullptr);
}

void RegPool::simulate(const std::vector<const symtable::Entry*>& _seq, 
		const std::vector<bool>& write, 
		const std::vector<bool>& mask) {
    assert(_actionCache.empty());
	Simulator simu(_reg_a, _reg_s, _seq, _actionCache);
	for (int i = 0; i < _seq.size(); i++) {
		simu.request(write[i], mask[i]);
	}
	simu.clear();
}

void RegPool::_execute(StackFrame& stackframe) {
	const Action* const action = _actionCache.front();
    _actionCache.pop();
    if (action->store != nullptr) {
        stackframe.store(action->reg, action->store);
	}
    if (action->load != nullptr) {
        stackframe.load(action->reg, action->load);
	}
    delete action;
}

Reg RegPool::request(StackFrame& stackframe) {
    const Reg result = _actionCache.front()->reg;
	_execute(stackframe);
    return result;
}

void RegPool::clear(StackFrame& stackframe) {
	while (!_actionCache.empty()) {
        assert(_actionCache.front()->load == nullptr && _actionCache.front()->store != nullptr);
		_execute(stackframe);
	}
}
