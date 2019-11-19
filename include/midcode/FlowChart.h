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
	symtable::FuncTable* _functable;
	std::vector<BasicBlock*> _blocks;
	BasicBlock* _tail;
public:
	const std::vector<BasicBlock*>& blocks(void) const;

	void _init(const symtable::FuncTable* const);
public:
	FlowChart(const symtable::FuncTable* const);
	FlowChart(symtable::FuncTable* const);
	~FlowChart(void);

	void commit(void);
};

#endif /* FLOW_CHART_H */
