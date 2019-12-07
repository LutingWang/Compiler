/**********************************************
    > File Name: varProp.cpp
    > Author: Luting Wang
    > Mail: 2457348692@qq.com 
    > Created Time: Fri Nov 29 15:27:38 2019
 **********************************************/

#include <cassert>
#include <string>
#include "midcode.h"

#include "./VarMatch.h"

#include "Optim.h"

using Instr = MidCode::Instr;

bool Optim::_varProp(const MidCode*& midcode, VarMatch& match) {
    const Instr instr = midcode->instr();
    const symtable::Entry* t0 = midcode->_t0;
    const symtable::Entry* t1 = midcode->_t1;
    const symtable::Entry* t2 = midcode->_t2;
    const std::string* t3 = midcode->_t3;
    bool update = match.contains(t1) || match.contains(t2);
    if (midcode->isCalc()) {
        t1 = match.map(t1);
        t2 = match.map(t2);
        match.erase(midcode->t0());
    } else if (midcode->isBranch()) {
        t1 = match.map(t1);
        t2 = match.map(t2);
        t3 = new std::string(midcode->labelName());
    } else switch (midcode->instr()) {
    case Instr::LOAD_IND:
        assert(!match.contains(t1));
        t2 = match.map(t2);
        match.erase(t0);
        break;
    case Instr::STORE_IND:
        t1 = match.map(t1);
        t2 = match.map(t2);
        assert(!match.contains(t0));
        break;
    case Instr::ASSIGN:
        t1 = match.map(t1);
        match.match(t0, t1);
        break;
    case Instr::PUSH_ARG:
    case Instr::RET:
    case Instr::OUTPUT_INT:
    case Instr::OUTPUT_CHAR:
        t1 = match.map(t1);
        break;
    case Instr::CALL:
        match.eraseGlobal(); // TODO: target those globals that would change in called func only
        // fallthrough
    case Instr::INPUT:
        match.erase(t0);
        break;
    default:
        break;
    }
    if (update) {
        delete midcode;
        midcode = new MidCode(instr, t0, t1, t2, t3);
    }
    return update;
}
