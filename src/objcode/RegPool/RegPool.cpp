/**********************************************
    > File Name: RegPool.cpp
    > Author: Luting Wang
    > Mail: 2457348692@qq.com 
    > Created Time: Sat Nov  9 00:00:58 2019
 **********************************************/

#include <cassert>
#include <set>
#include <vector>
#include "midcode.h"
#include "symtable.h"

#include "Action.h"
#include "Simulator.h"

#include "../include/RegPool.h"
#include "../include/memory.h"

RegPool::RegPool(const std::vector<MidCode*>& midcode, 
		const std::vector<symtable::Entry*>& reg_a) : 
	_reg_a(reg_a) {
	assert(_reg_a.size() == reg::a.size());

	// TODO: assign saved registers
	_reg_s = std::vector<symtable::Entry*>(8, nullptr);
}

RegPool::~RegPool(void) {
    for (auto& actions : _actionCache) {
        for (auto& action : actions) {
            delete action;
        }
    }
}

void RegPool::simulate(const std::vector<symtable::Entry*>& _seq, 
		const std::vector<bool>& write, 
		const std::vector<bool>& mask) {
    _actionCache.push_back({});
	Simulator simu(_reg_a, _reg_s, _seq, _actionCache.back());
	for (int i = 0; i < _seq.size(); i++) {
		simu.request(write[i], mask[i]);
	}
	simu.clear();
}

void RegPool::storage(std::set<symtable::Entry*>& stack) const {
	for (auto& actions : _actionCache) {
		for (auto& action : actions) {
            if (action->store != nullptr && !action->store->isConst) {
                stack.insert(action->store);
            }
            if (action->load != nullptr && !action->load->isConst) {
                stack.insert(action->load);
            }
		}
	}
	stack.erase(nullptr);
}

void RegPool::_execute(StackFrame& stackframe) {
	Action* action = _actionCache[_blockCounter][_actionCounter];
    if (action->store != nullptr) {
        stackframe.store(action->reg, action->store);
	}
    if (action->load != nullptr) {
        stackframe.load(action->reg, action->load);
	}
}

Reg RegPool::request(StackFrame& stackframe) {
	_execute(stackframe);
	Action* action = _actionCache[_blockCounter][_actionCounter];
	_actionCounter++;
    return action->reg;
}

void RegPool::clear(StackFrame& stackframe) {
	while (_actionCounter < _actionCache[_blockCounter].size()) {
		_execute(stackframe);
		_actionCounter++;
	}
	_blockCounter++;
    _actionCounter = 0;
}
