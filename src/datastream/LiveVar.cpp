/**********************************************
    > File Name: LiveVar.cpp
    > Author: Luting Wang
    > Mail: 2457348692@qq.com 
    > Created Time: Thu Dec  5 10:30:24 2019
 **********************************************/

#include <cassert>
#include "midcode/MidCode.h"
#include "symtable/Entry.h"

#include "datastream/LiveVar.h"

using Instr = MidCode::Instr;

void LiveVar::use(std::vector<const symtable::Entry*>& output, const MidCode* const midcode) {
    assert(output.empty());
    // note that t1 could be array in `LOAD_IND`
    if (midcode->t1IsValid() && !midcode->t1()->isArray()) {
        output.push_back(midcode->t1());
    }
    if (midcode->t2IsValid()) {
        output.push_back(midcode->t2());
    }
}

void LiveVar::def(symtable::Entry const*& output, const MidCode* const midcode) {
    assert(output == nullptr);
    // note that t0 could be array in `STORE_IND`
    if (midcode->t0IsValid() && !midcode->t0()->isArray()) {
        output = midcode->t0();
    }
}
