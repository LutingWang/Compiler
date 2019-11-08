/**********************************************
    > File Name: FlowChart.h
    > Author: Luting Wang
    > Mail: 2457348692@qq.com 
    > Created Time: Fri Nov  8 09:20:00 2019
 **********************************************/

#ifndef FLOW_CHART_H
#define FLOW_CHART_H

#include <vector>

class MidCode;

class BasicBlock;

class FlowChart {
public:
	std::vector<BasicBlock*> blocks;

	FlowChart(const std::vector<MidCode*>&);

	// deallocate `blocks`
	~FlowChart(void);

	// merge all the mid codes in `blocks` into 
	// one vector and return
	std::vector<MidCode*> output(void) const;
};

#endif /* FLOW_CHART_H */
