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

class MidCode;
class FlowChart;

class BasicBlock {
	friend class FlowChart;

	std::vector<const MidCode*> _midcodes;
	std::set<BasicBlock*> _prec; // precursors
	std::set<BasicBlock*> _succ; // successors
	bool _dirty = false;
public:
	const std::vector<const MidCode*>& midcodes(void) const;
	bool dirty(void) const;

private:
	BasicBlock(void);
	BasicBlock(const std::vector<const MidCode*>::const_iterator, 
			const std::vector<const MidCode*>::const_iterator);
	~BasicBlock(void);

	void _proceed(BasicBlock* const);
	void _unlink(void);
public:
	bool isFuncCall(void) const;
	bool isolated(void) const;
};

#endif /* BASIC_BLOCK_H */
