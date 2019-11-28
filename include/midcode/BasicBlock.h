/**********************************************
    > File Name: BasicBlock.h
    > Author: Luting Wang
    > Mail: 2457348692@qq.com 
    > Created Time: Sat Nov 16 21:37:47 2019
 **********************************************/

#ifndef BASIC_BLOCK_H
#define BASIC_BLOCK_H

#include <set>
#include <vector>

class Optim;

class MidCode;
class FlowChart;

class BasicBlock {
	friend class FlowChart;
	friend class Optim;

	std::vector<const MidCode*> _midcodes;
	std::set<BasicBlock*> _prec; // precursors
	std::set<BasicBlock*> _succ; // successors
public:
	const std::vector<const MidCode*>& midcodes(void) const;

private:
	BasicBlock(void);
	BasicBlock(const std::vector<const MidCode*>::const_iterator, 
			const std::vector<const MidCode*>::const_iterator);
	~BasicBlock(void);

    // link another block to the back
	void _proceed(BasicBlock* const);
public:
    // A call block contains multiple `PUSH` statements and
    // a single `CALL` statement. If a block is not a call
    // block, then it cannot contain these two kinds of statements.
	bool isFuncCall(void) const;
};

#endif /* BASIC_BLOCK_H */
