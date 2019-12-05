/**********************************************
    > File Name: Simulator.cpp
    > Author: Luting Wang
    > Mail: 2457348692@qq.com 
    > Created Time: Sun Nov 10 13:35:56 2019
 **********************************************/

#include <algorithm>
#include <climits>

#include "Action.h"

#include "Simulator.h"

#define NAN -1

Simulator::Simulator(ActionGen& output,
        const std::vector<const symtable::Entry*>& reg_a,
		const std::vector<const symtable::Entry*>& reg_s, 
		const std::vector<const symtable::Entry*>& _seq) :
	_reg_a(reg_a), 
	_reg_s(reg_s), 
	_reg_t(reg::t.size(), nullptr), 
	_dirty(reg::t.size(), false),
    _maskCache(NAN),
	_seq(_seq), 
	_output(output) {}

void Simulator::request(bool write, bool mask) {
    const symtable::Entry* const target = _seq.front();
    _seq.erase(_seq.begin());

	// check a regs
	int ind = std::find(_reg_a.begin(), _reg_a.end(), target) - _reg_a.begin();
	if (ind != _reg_a.size()) {
        _maskCache = NAN;
		_output(reg::a[ind], nullptr, nullptr);
		return;
	}

	// check s regs
	ind = std::find(_reg_s.begin(), _reg_s.end(), target) - _reg_s.begin();
    if (ind != _reg_s.size()) {
        _maskCache = NAN;
        _output(reg::s[ind], nullptr, nullptr);
        return;
    }

	// check t regs
    ind = std::find(_reg_t.begin(), _reg_t.end(), target) - _reg_t.begin();
    if (ind != _reg_t.size()) {
        _maskCache = ind;
        _output(reg::t[ind], nullptr, nullptr);
		if (write) { _dirty[ind] = true; }
        return;
    }

	// try to find a nullptr in the temporary registers
    ind = std::find(_reg_t.begin(), _reg_t.end(), nullptr) - _reg_t.begin();
    if (ind != _reg_t.size()) {
        _maskCache = ind;
        _output(reg::t[ind], write ? nullptr : target, nullptr);
        _reg_t[ind] = target;
        _dirty[ind] = write;
        return;
    }

	// use LRU strategy
	std::vector<int> usage(_reg_t.size(), INT_MAX);
    int appeared = 0; // if all `Entry`s have appeared, then stop the iteration
	for (int i = 0; i < _seq.size(); i++) {
		ind = std::find(_reg_t.begin(), _reg_t.end(), _seq[i]) - _reg_t.begin();
        if (ind == _reg_t.size() || usage[ind] < i) { continue; }
        usage[ind] = i;
        if (++appeared == usage.size()) { break; }
	}
    
    // if `mask` is on and valid, disqualify the corresponding reg
    if (mask && _maskCache != NAN) { usage[_maskCache] = 0; }
    
    // find the register to be used last
    ind = std::max_element(usage.begin(), usage.end()) - usage.begin();
    _maskCache = ind;
    _output(reg::t[ind], write ? nullptr : target, _dirty[ind] ? _reg_t[ind] : nullptr);
    _reg_t[ind] = target;
    _dirty[ind] = write;
}

void Simulator::clear(void) {
	for (int i = 0; i < _reg_t.size(); i++) {
		if (_dirty[i]) {
            _output(reg::t[i], nullptr, _reg_t[i]);
        }
	}
}
