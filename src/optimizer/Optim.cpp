/**********************************************
    > File Name: Optim.cpp
    > Author: Luting Wang
    > Mail: 2457348692@qq.com 
    > Created Time: Tue Nov 19 10:36:03 2019
 **********************************************/

#include "midcode/MidCode.h"
#include "midcode/BasicBlock.h"
#include "symtable/SymTable.h"

#include "Optim.h"

const symtable::FuncTable* Optim::_calledFunc(const BasicBlock* const basicblock) {
    return SymTable::getTable().findFunc(basicblock->midcodes().back()->labelName());
}
