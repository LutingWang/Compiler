/**********************************************
    > File Name: BasicBlock.h
    > Author: Luting Wang
    > Mail: 2457348692@qq.com 
    > Created Time: Fri Nov  8 09:12:04 2019
 **********************************************/

#ifndef BASIC_BLOCK_H
#define BASIC_BLOCK_H

#include <set>
#include <vector>

class MidCode;

class BasicBlock {
public:
	std::vector<MidCode*> midcodes;
	std::set<BasicBlock*> prec; // precursors
	std::set<BasicBlock*> succ; // successors

	BasicBlock(const std::vector<MidCode*>&);

	// Add a basic block to `_successors` and
	// add `this` to `_precursors` of the basic
	// block.
	void proceed(BasicBlock* const);
};

#endif /* BASIC_BLOCK_H */
