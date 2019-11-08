/**********************************************
    > File Name: Optim.h
    > Author: Luting Wang
    > Mail: 2457348692@qq.com 
    > Created Time: Wed Nov  6 22:11:33 2019
 **********************************************/

#ifndef OPTIM_H
#define OPTIM_H

#include <map>
#include <string>

class FlowChart;

class Optim {
	FlowChart* _mainFlow;

	// <func name, func flow chart>
	std::map<std::string, FlowChart*> _chart;

	// build `_chart` from `table`
	Optim(void);

	// Write back to `table`
	~Optim(void);
public:
	static void inlineExpan(void);
	static void commonExprElim(void);
};

#endif /* OPTIM_H */
