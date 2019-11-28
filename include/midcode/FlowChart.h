/**********************************************
    > File Name: FlowChart.h
    > Author: Luting Wang
    > Mail: 2457348692@qq.com 
    > Created Time: Sat Nov 16 21:31:21 2019
 **********************************************/

#ifndef FLOW_CHART_H
#define FLOW_CHART_H

#include <vector>

namespace symtable {
	class FuncTable;
}

class BasicBlock;

class FlowChart {
    // The destiny of midcode writing back. If the flowchart
    // is declared as constant, then its midcodes cannot be
    // altered. In this case, `_functable` is set as `nullptr`
    // to avoid writing back.
	symtable::FuncTable* _functable;
    
    // Sequentially stored midcodes in the form of `BasicBlock`s.
	std::vector<BasicBlock*> _blocks;
    // The last empty block that all return statements jump to.
	BasicBlock* _tail;
public:
	const std::vector<BasicBlock*>& blocks(void) const;

	void _init(const symtable::FuncTable* const);
public:
	FlowChart(const symtable::FuncTable* const);
	FlowChart(symtable::FuncTable* const);
    
    // Deallocate all the blocks. Do not forget `_tail`.
	~FlowChart(void);

    // Write back to cover the original midcodes in `_functable`.
	void commit(void);
};

#endif /* FLOW_CHART_H */
