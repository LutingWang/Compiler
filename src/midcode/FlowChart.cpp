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
#include "midcode/MidCode.h"
#include "midcode/BasicBlock.h"
#include "midcode/FlowChart.h"
#include "symtable/table.h"

const std::vector<BasicBlock*>& FlowChart::blocks(void) const {
    return _blocks;
}

void FlowChart::_init(const symtable::FuncTable* const functable) {
    auto& midcodes = functable->midcodes();
    
    // initialize <label name, label pos in mid code>
    std::map<std::string, int> labels;
    for (int i = 0; i < midcodes.size(); i++) {
        if (midcodes[i]->is(MidCode::Instr::LABEL)) {
            labels[midcodes[i]->labelName()] = i;
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
    for (int i = 0; i < midcodes.size(); i++) {
        switch (midcodes[i]->instr) {
        case MidCode::Instr::PUSH_ARG:
        case MidCode::Instr::CALL:
            entrances.insert(i);
            while (!midcodes[i]->is(MidCode::Instr::CALL)) { i++; }
            entrances.insert(i + 1);
            break;
        case MidCode::Instr::BGT:
        case MidCode::Instr::BGE:
        case MidCode::Instr::BLT:
        case MidCode::Instr::BLE:
        case MidCode::Instr::BEQ:
        case MidCode::Instr::BNE:
        case MidCode::Instr::GOTO:
            entrances.insert(labels[midcodes[i]->labelName()]);
            // fallthrough
        case MidCode::Instr::RET:
            entrances.insert(i + 1);
            break;
        default: continue;
        }
    }
    entrances.erase(0); // the first mid code has to be entrance no matter what
    entrances.insert(midcodes.size()); // set up guard element
    assert(*(entrances.rbegin()) == midcodes.size());

    // Divide the mid codes into blocks. If a block
    // starts with a label, add it to the map to
    // enable other blocks to discover it.
    std::map<std::string, int> blockMap;
    int startIndex = 0;
    for (auto& endIndex : entrances) {
        if (midcodes[startIndex]->is(MidCode::Instr::LABEL)) {
            blockMap[midcodes[startIndex]->labelName()] = blocks().size();
        }
        _blocks.push_back(
            new BasicBlock(
                midcodes.begin() + startIndex,
                midcodes.begin() + endIndex
            )
        );
        startIndex = endIndex;
    }

    // Scan the blocks to store flow information.
    // For return statements, proceed to `_tail`.
    _tail = new BasicBlock();
    for (int i = 0; i < blocks().size(); i++) {
        const MidCode* exitcode = blocks()[i]->midcodes().back();
        switch (exitcode->instr) {
        case MidCode::Instr::RET:
            blocks()[i]->_proceed(_tail);
            break;
        case MidCode::Instr::GOTO:
            assert(blockMap.find(exitcode->labelName()) != blockMap.end());
            blocks()[i]->_proceed(blocks()[blockMap[exitcode->labelName()]]);
            break;
        case MidCode::Instr::BGT:
        case MidCode::Instr::BGE:
        case MidCode::Instr::BLT:
        case MidCode::Instr::BLE:
        case MidCode::Instr::BEQ:
        case MidCode::Instr::BNE:
            assert(blockMap.find(exitcode->labelName()) != blockMap.end());
            blocks()[i]->_proceed(blocks()[blockMap[exitcode->labelName()]]);
            // fallthrough
        default:
            if (i + 1 < blocks().size()) {
                blocks()[i]->_proceed(blocks()[i + 1]);
            }
        }
    }
}

FlowChart::FlowChart(const symtable::FuncTable* const functable) :
    _functable(nullptr) {
    _init(functable);
}

FlowChart::FlowChart(symtable::FuncTable* const functable) :
    _functable(functable) {
    _init(functable);
    
    // iterate over and delete those blocks that can never be visited
    for (int i = 1; i < blocks().size(); i++) {
        if (!blocks()[i]->isolated()) { continue; }
        blocks()[i]->_unlink();
        for (auto& midcode : blocks()[i]->midcodes()) {
            delete midcode;
        }

        // delete from `blocks`
        delete blocks()[i];
        _blocks.erase(blocks().begin() + i);

        // re-examine all the remaining blocks
        i = 0;
    }
}

FlowChart::~FlowChart(void) {
    assert(!_dirty());
    for (auto& block : blocks()) {
        delete block;
    }
    delete _tail;
}

bool FlowChart::_dirty(void) const {
    for (auto block : _blocks) {
        if (block->dirty()) {
            return true;
        }
    }
    return false;
}

void FlowChart::commit(void) {
    _functable->_midcodes.clear();
    for (auto block : blocks()) {
        block->_dirty = false;
		_functable->_midcodes.insert(
				_functable->_midcodes.end(), 
				block->midcodes().begin(), 
				block->midcodes().end()
		);
	}
}
