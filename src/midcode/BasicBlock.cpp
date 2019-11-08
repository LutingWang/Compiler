/**********************************************
    > File Name: BasicBlock.cpp
    > Author: Luting Wang
    > Mail: 2457348692@qq.com 
    > Created Time: Fri Nov  8 09:40:40 2019
 **********************************************/

#include <set>
#include <vector>
#include "midcode.h"

BasicBlock::BasicBlock(const std::vector<MidCode*>& midcodes) : midcodes(midcodes) {}

void BasicBlock::proceed(BasicBlock* const successor) {
	succ.insert(successor);
	successor->prec.insert(this);
}
