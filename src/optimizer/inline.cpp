/**********************************************
    > File Name: inline.cpp
    > Author: Luting Wang
    > Mail: 2457348692@qq.com 
    > Created Time: Wed Nov  6 22:13:52 2019
 **********************************************/

#include <cassert>
#include <string>
#include <vector>
#include "midcode/MidCode.h"
#include "symtable/SymTable.h"

#include "Optim.h"

void Optim::inlineExpan(void) {
//	std::vector<MidCode*>& mc = table._main._midcode;
//	for (int i = 0; i < mc.size(); i++) {
//		if (mc[i]->instr != MidCode::Instr::PUSH_ARG &&
//				mc[i]->instr != MidCode::Instr::CALL) {
//			continue;
//		}
//
//		int callPos; // position of call statment
//		for (callPos = i; mc[callPos]->instr == MidCode::Instr::PUSH_ARG; callPos++);
//		assert(mc[callPos]->instr == MidCode::Instr::CALL);
//
//		const symtable::FuncTable* ft = table.findFunc(mc[callPos]->t3);
//		if (!ft->isInline()) {
//			i = callPos;
//			continue; // i++
//		}
//
//		// iterate over all push statements and convert to assign
//        const std::vector<symtable::Entry*>& argv = ft->argList();
//		for (int k = i; k < callPos; k++) {
//			MidCode* arg2assign = new MidCode(MidCode::Instr::ASSIGN,
//					argv[k - i], mc[k]->t1, nullptr, "");
//			delete mc[k];
//			mc[k] = arg2assign;
//		}
//
//		// Deep clone the mid code. Since this loop will delete
//		// all the push statments, shallow copy would result in
//		// the corresponding mid code in `ft` being wild.
//		std::vector<MidCode*> dupFunc = ft->_midcode;
//		for (int k = 0; k < dupFunc.size(); k++) {
//			dupFunc[k] = new MidCode(*(dupFunc[k]));
//		}
//
//		// replace return statements with assign and goto
//		std::string funcEnd = MidCode::genLabel();
//		for (int k = 0; k < dupFunc.size(); k++) {
//			if (dupFunc[k]->instr != MidCode::Instr::RET) { continue; }
//			if (!ft->isVoid) {
//				dupFunc.insert(dupFunc.begin() + k, new MidCode(MidCode::Instr::ASSIGN,
//							mc[callPos]->t0, dupFunc[k]->t1, nullptr, ""));
//				k++;
//			}
//			dupFunc[k] = new MidCode(MidCode::Instr::GOTO, nullptr, nullptr, nullptr, funcEnd);
//		}
//		dupFunc.push_back(new MidCode(MidCode::Instr::LABEL, nullptr, nullptr, nullptr, funcEnd));
//
//		// replace call with duplicated function
//		mc.erase(mc.begin() + callPos);
//		mc.insert(mc.begin() + callPos, dupFunc.begin(), dupFunc.end());
//
//		i = callPos - 1;
//	}
}
