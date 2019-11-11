/**********************************************
    > File Name: RegPool.cpp
    > Author: Luting Wang
    > Mail: 2457348692@qq.com 
    > Created Time: Sat Nov  9 00:00:58 2019
 **********************************************/

#include <set>
#include <vector>
#include "midcode.h"
#include "symtable.h"

#include "Action.h"
#include "Simulator.h"

#include "../include/RegPool.h"
#include "../include/stackframe.h"

RegPool::RegPool(const std::vector<MidCode*>& midcode, 
		const std::vector<symtable::Entry*>& reg_a) : _reg_a(reg_a) {
	assignRegS(midcode);

	FlowChart flowchart(midcode);
	for (auto& basicblock : flowchart.blocks) {
		std::vector<symtable::Entry*> _seq;
		std::vector<bool> write;
		std::vector<bool> mask;
		for (auto& mc : basicblock->midcodes) {
			switch (mc->instr) {
#ifdef CASE
	#error macro conflict
#endif /* CASE */
#define CASE(id) case MidCode::Instr::id

#ifdef PUSH
	#error macro conflict
#endif /* PUSH */
#define PUSH(t, w, m) \
				_seq.push_back(mc->t); \
				write.push_back(w); \
				mask.push_back(m)

#ifdef PUSH_T0
	#error macro conflict
#endif /* PUSH_T0 */
#define PUSH_T0 PUSH(t0, true, false)

#ifdef PUSH_T1
	#error macro conflict
#endif /* PUSH_T1 */
#define PUSH_T1 PUSH(t1, false, false)

#ifdef PUSH_T2
	#error macro conflict
#endif /* PUSH_T2 */
#define PUSH_T2 PUSH(t2, false, true)

			CASE(ADD): CASE(SUB): CASE(MULT): CASE(DIV): 
			CASE(LOAD_IND): CASE(STORE_IND):
				PUSH_T1; PUSH_T2; PUSH_T0;
				break;
			CASE(ASSIGN): 
				PUSH_T1; PUSH_T0; 
				break;
			CASE(PUSH_ARG): 
				PUSH_T0; PUSH_T1; 
				break;
			CASE(CALL): 
				if (mc->t0 != nullptr) { PUSH_T0; } 
				break;
			CASE(RET): CASE(OUTPUT): 
				if (mc->t1 != nullptr) { PUSH_T1; } 
				break;
			CASE(INPUT): 
				PUSH_T0; 
				break;
			CASE(BGT): CASE(BGE): CASE(BLT): CASE(BLE):
				PUSH_T1; PUSH_T2;
				break;
			CASE(BEQ): CASE(BNE):
				PUSH_T1;
				if (mc->t2 != nullptr) { PUSH_T2; }
				break;
			default: continue;
#undef CASE
#undef PUSH
#undef PUSH_T0
#undef PUSH_T1
#undef PUSH_T2
			}
		}

		_actions.emplace_back();
		Simulator simu(_reg_a, _reg_s, _seq, _actions.back());
		for (int i = 0; i < _seq.size(); i++) {
			simu.request(write[i], mask[i]);
		}
		simu.clear();
	}
}

// TODO: complete me
void RegPool::assignRegS(const std::vector<MidCode*>& midcode) {
	_reg_s = std::vector<symtable::Entry*>(reg::s.size(), nullptr);
}

void RegPool::storage(std::set<symtable::Entry*>& stack) const {
	stack.clear();
	for (auto& actions : _actions) {
		for (auto& action : actions) {
            stack.insert(action.store);
            stack.insert(action.load);
		}
	}
	stack.erase(nullptr);
}

void RegPool::execute(const StackFrame& stackframe) {
	Action& action = _actions[_blockCounter][_actionCounter];
	if (action.store != nullptr) {
		if (action.store->isConst) {
			// TODO: generate li
		} else {
			stackframe.store(action.reg, action.store);
		}
	}
	if (action.load != nullptr) {
		if (action.store->isConst) {
			// TODO: generate li
		} else {
			stackframe.load(action.reg, action.load);
		}
	}
}

Reg RegPool::request(const StackFrame& stackframe) {
	execute(stackframe);
	Action& action = _actions[_blockCounter][_actionCounter];
	_actionCounter++;
	return action.reg;
}

void RegPool::clear(const StackFrame& stackframe) {
	while (_actionCounter < _actions[_blockCounter].size()) {
		execute(stackframe);
		_actionCounter++;
	}
	_blockCounter++;
}
