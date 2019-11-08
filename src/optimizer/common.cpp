/**********************************************
    > File Name: common.cpp
    > Author: Luting Wang
    > Mail: 2457348692@qq.com 
    > Created Time: Thu Nov  7 20:25:38 2019
 **********************************************/

#include <map>
#include <queue>
#include <string>
#include <set>
#include <vector>
#include "symtable.h"
#include "MidCode.h"

#include "Optim.h"

void Optim::commonExprElim(void) {
	std::queue<std::string> funcs;
	for (funcs.push("main"); !funcs.empty(); funcs.pop()) {
		const std::vector<MidCode*>& mc = table.findFunc(funcs.front())->_midcode;

		std::map<std::string, int> labels;
		for (int i = 0; i < mc.size(); i++) {
			if (mc[i]->instr == MidCode::Instr::LABEL) { 
				labels[mc[i]->t3] = i;
			}
		}

		std::set<int> entrances;
		for (int i = 0; i < mc.size(); i++) {
			switch (mc[i]->instr) {
			case MidCode::Instr::CALL:
				funcs.push(mc[i]->t3);
				entrances.insert(i + 1);
				break;
			case MidCode::Instr::BGT:
			case MidCode::Instr::BGE:
			case MidCode::Instr::BLT:
			case MidCode::Instr::BLE:
			case MidCode::Instr::BEQ:
			case MidCode::Instr::BNE:
			case MidCode::Instr::GOTO:
				entrances.insert(i + 1);
				entrances.insert(labels[mc[i]->t3]);
				break;
			default: continue;
			}
		}
		entrances.erase(mc.size());
	}
}
