/**********************************************
    > File Name: BasicBlock.cpp
    > Author: Luting Wang
    > Mail: 2457348692@qq.com 
    > Created Time: Fri Nov  8 09:40:40 2019
 **********************************************/

#include <cassert>
#include <set>
#include <vector>
#include "midcode/MidCode.h"

#include "midcode/BasicBlock.h"

const std::vector<const MidCode*>& BasicBlock::midcodes(void) const {
    return _midcodes;
}

BasicBlock::BasicBlock(void) {}

BasicBlock::BasicBlock(const std::vector<const MidCode*>::const_iterator first,
        const std::vector<const MidCode*>::const_iterator last) :
    _midcodes(first, last) {}

BasicBlock::~BasicBlock(void) {}

void BasicBlock::_proceed(BasicBlock* const successor) {
	_succ.insert(successor);
	successor->_prec.insert(this);
}

bool BasicBlock::isFuncCall(void) const {
	return _midcodes.back()->is(MidCode::Instr::CALL);
}
