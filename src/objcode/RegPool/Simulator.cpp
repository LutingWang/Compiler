/**********************************************
    > File Name: Simulator.cpp
    > Author: Luting Wang
    > Mail: 2457348692@qq.com 
    > Created Time: Sun Nov 10 13:35:56 2019
 **********************************************/

#include <algorithm>
#include "../include/Reg.h"

#include "Action.h"

#include "Simulator.h"

Simulator::Simulator(const std::vector<symtable::Entry*>& reg_a, 
		const std::vector<symtable::Entry*>& reg_s, 
		const std::vector<symtable::Entry*>& _seq, 
		std::vector<Action>& actions) :
	_reg_a(reg_a), 
	_reg_s(reg_s), 
	_reg_t(reg::t.size(), nullptr), 
	_dirty(reg::t.size(), false),
	_seq(_seq), 
	_actions(actions) {}

void Simulator::request(bool write, bool mask) {
	const symtable::Entry* target = _seq[_counter];
	_counter++;

	// check a registers
	int ind = std::find(_reg_a.begin(), _reg_a.end(), target) - _reg_a.begin();
	if (ind != _reg_a.size()) {
		_actions.emplace_back(reg::a[ind]);
		return;
	}

	// check s registers
	ind = std::find(_reg_s.begin(), _reg_s.end(), target) - _reg_s.begin();
    if (ind != _reg_s.size()) {
        _actions.emplace_back(reg::s[ind]);
        return;
    }

	// check temporary registers
    ind = std::find(_reg_t.begin(), _reg_t.end(), target) - _reg_t.begin();
    if (ind != _reg_t.size()) {
        _actions.emplace_back(reg::t[ind]);
		if (write) { _dirty[ind] = true; }
        return;
    }

	// try to find a nullptr in the temporary registers
    ind = std::find(_reg_t.begin(), _reg_t.end(), nullptr) - _reg_t.begin();
    if (ind != _reg_t.size()) {
        _actions.emplace_back(reg::t[ind], target);
		if (write) { _dirty[ind] = true; }
        return;
    }

	// use LRU strategy
	std::vector<int> usage(_reg_t.size(), INT_MAX);
    int appeared = 0; // if all `Entry`s have appeared, then stop the iteration
	for (int i = _counter; i < _seq.size(); i++) {
		ind = std::find(_reg_t.begin(), _reg_t.end(), _seq[i]) - _reg_t.begin();
        if (ind == _reg_t.size() || usage[ind] < i) { continue; }
        usage[ind] = i;
        if (++appeared == usage.size()) { break; }
	}
    
    // if `mask` is on and valid, disqualify the corresponding reg
    if (mask) {
        ind = std::find(reg::t.begin(), reg::t.end(), _actions.back().reg) - reg::t.begin();
        if (ind != reg::t.size()) { usage[ind] = 0; }
    }
    
    // find the register with latest usage
    ind = std::max_element(usage.begin(), usage.end()) - usage.begin();
    _actions.emplace_back(reg::t[ind], target, _dirty[ind] ? _reg_t[ind] : nullptr);
	if (write) { _dirty[ind] = true; }
}

void Simulator::clear(void) {
	for (int i = 0; i < _reg_t.size(); i++) {
		if (_dirty[i]) { _actions.emplace_back(reg::t[i], nullptr, _reg_t[i]); }
	}
}
