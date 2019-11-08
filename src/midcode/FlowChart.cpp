/**********************************************
    > File Name: FlowChart.cpp
    > Author: Luting Wang
    > Mail: 2457348692@qq.com 
    > Created Time: Fri Nov  8 09:40:59 2019
 **********************************************/

#include <cassert>
#include <map>
#include <set>
#include <string>
#include <vector>
#include "midcode.h"

FlowChart::FlowChart(const std::vector<MidCode*>& mc) {
	// initialize <label name, label pos in mid code>
	std::map<std::string, int> labels;
	for (int i = 0; i < mc.size(); i++) {
		if (mc[i]->instr == MidCode::Instr::LABEL) { 
			labels[mc[i]->t3] = i;
		}
	}

	// Scan the mid code to determine pos of entrances.
	// An entrance can be one of the following
	//     - the first mid code of all
	//     - the first push or call statements in a function call block
	//     - the first statement after 
	//         1. a call block
	//         2. a return statement
	//         3. a branch statement
	//         4. a goto statement
	//     - the statement jumped to from
	//         1. a branch statement
	//         2. a goto statement
	std::set<int> entrances;
	for (int i = 0; i < mc.size(); i++) {
		switch (mc[i]->instr) {
		case MidCode::Instr::PUSH_ARG:
		case MidCode::Instr::CALL:
			entrances.insert(i);
			while (mc[i]->instr != MidCode::Instr::CALL) { i++; }
			// fallthrough
        case MidCode::Instr::RET:
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
	entrances.erase(0); // the first mid code has to be entrance no matter what
	entrances.insert(mc.size()); // set up guard element
	assert(*(entrances.rbegin()) == mc.size());

	// Divide the mid codes into blocks. If a block 
	// starts with a label, add it to the map to 
	// enable other blocks to discover it.
	std::map<std::string, int> blockMap; 
	int startIndex = 0;
	for (auto& endIndex : entrances) {
		if (mc[startIndex]->instr == MidCode::Instr::LABEL) {
			blockMap[mc[startIndex]->t3] = blocks.size();
		}
		blocks.push_back(
			new BasicBlock(
				std::vector<MidCode*>(
					mc.begin() + startIndex, 
					mc.begin() + endIndex
				)
			)
		);
		startIndex = endIndex;
	}

	// Scan the blocks to store flow information.
	// For return statements, proceed to `_tail`.
	BasicBlock* tail = new BasicBlock({});
	for (int i = 0; i < blocks.size(); i++) {
		MidCode* exitcode = blocks[i]->midcodes.back();
		switch (exitcode->instr) {
		case MidCode::Instr::RET:
			blocks[i]->proceed(tail);
			break;
		case MidCode::Instr::GOTO:
			assert(blockMap.find(exitcode->t3) != blockMap.end());
			blocks[i]->proceed(blocks[blockMap[exitcode->t3]]);
			break;
		case MidCode::Instr::BGT:
		case MidCode::Instr::BGE:
		case MidCode::Instr::BLT:
		case MidCode::Instr::BLE:
		case MidCode::Instr::BEQ:
		case MidCode::Instr::BNE:
			assert(blockMap.find(exitcode->t3) != blockMap.end());
			blocks[i]->proceed(blocks[blockMap[exitcode->t3]]);
			// fallthrough
		default:
			if (i + 1 < blocks.size()) {
				blocks[i]->proceed(blocks[i + 1]);
			}
		}
	}
	blocks.push_back(tail);

	// iterate over and delete those blocks that can never be visited
	for (int i = 1; i < blocks.size(); i++) {
		if (!blocks[i]->prec.empty()) { continue; }

		// break linkages
		BasicBlock* const target = blocks[i];
		for (auto& block : target->succ) {
			block->prec.erase(target);
		}
		for (auto& mc : target->midcodes) {
			delete mc;
		}

		// delete from `blocks`
		blocks.erase(blocks.begin() + i);

		// re-examine all the remaining blocks
		i = 0;
	}
}

FlowChart::~FlowChart(void) {
	for (auto& block : blocks) { delete block; }
}

std::vector<MidCode*> FlowChart::output(void) const {
	std::vector<MidCode*> result;
	for (auto& block : blocks) {
		result.insert(result.end(), block->midcodes.begin(), block->midcodes.end());
	}
	return result;
}
