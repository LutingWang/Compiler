/**********************************************
    > File Name: propagation.cpp
    > Author: Luting Wang
    > Mail: 2457348692@qq.com 
    > Created Time: Thu Nov 28 19:59:03 2019
 **********************************************/

#include <set>
#include "midcode/MidCode.h"
#include "midcode/BasicBlock.h"
#include "midcode/FlowChart.h"
#include "symtable/table.h"
#include "symtable/SymTable.h"

#include "Optim.h"

void Optim::symProp(void) {
    std::set<symtable::FuncTable*> funcs;
    SymTable::getTable().funcs(funcs, false);
    for (auto functable : funcs) {
	}
}
