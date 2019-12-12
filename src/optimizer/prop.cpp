/**********************************************
    > File Name: prop.cpp
    > Author: Luting Wang
    > Mail: 2457348692@qq.com 
    > Created Time: Thu Nov 28 19:59:03 2019
 **********************************************/

#include <cassert>
#include <map>
#include <set>
#include <string>
#include "datastream.h"
#include "midcode.h"
#include "symtable.h"

#include "Optim.h"
using Instr = MidCode::Instr;

/* constProp */

bool Optim::_constProp(const MidCode*& midcode) {
    // store ind cannot be optimized
    if (midcode->is(Instr::STORE_IND)) {
        return false;
    }
    if (!midcode->t1IsValid() || !midcode->t2IsValid()) {
        return false;
    }
    if (!midcode->t1()->isConst() ||!midcode->t2()->isConst()) {
        return false;
    }
    
#define eval(op) (midcode->t1()->value() op midcode->t2()->value())
    int* value = nullptr;
    bool* cond = nullptr;
    switch (midcode->instr()) {
    case Instr::ADD: value = new int(eval(+)); break;
    case Instr::SUB: value = new int(eval(-)); break;
    case Instr::MULT: value = new int(eval(*)); break;
    case Instr::DIV: value = new int(eval(/)); break;
    case Instr::BGT: cond = new bool(eval(>)); break;
    case Instr::BGE: cond = new bool(eval(>=)); break;
    case Instr::BLT: cond = new bool(eval(<)); break;
    case Instr::BLE: cond = new bool(eval(<=)); break;
    case Instr::BEQ: cond = new bool(eval(==)); break;
    case Instr::BNE: cond = new bool(eval(!=)); break;
    default: assert(0);
    }
    
    const MidCode* newCode = nullptr;
    assert((value == nullptr) != (cond == nullptr));
    if (value != nullptr) {
        newCode = new MidCode(MidCode::Instr::ASSIGN, midcode->t0(),
                MidCode::genConst(true, *value), nullptr, nullptr);
    } else if (*cond) {
        newCode = new MidCode(MidCode::Instr::GOTO, nullptr, nullptr,
                nullptr, new std::string(midcode->labelName()));
    }
    delete value;
    delete cond;
    delete midcode;
    midcode = newCode;
    return true;
}

/* VarMatch */

class VarMatch {
    std::map<const symtable::Entry*, const symtable::Entry*> _matches;
public:
    VarMatch(const Defs&);
    
    bool contains(const symtable::Entry* const) const;
    const symtable::Entry* map(const symtable::Entry* const) const;
    void erase(const symtable::Entry* const);
    void eraseGlobal(void);
    void match(const symtable::Entry* const, const symtable::Entry* const);
};

VarMatch::VarMatch(const Defs& defs) {
    std::set<const symtable::Entry*> blackList;
    for (auto midcode : defs) {
        auto t0 = midcode->t0();
        assert(!t0->isGlobal() && !t0->isArray() && !t0->isConst());
        if (!midcode->is(MidCode::Instr::ASSIGN) || _matches.count(t0) || !midcode->t1()->isConst()) {
            _matches.erase(t0);
            blackList.insert(t0);
        } else if (!blackList.count(t0)) {
            _matches[t0] = midcode->t1();
        }
    }
}

bool VarMatch::contains(const symtable::Entry* const entry) const {
    assert(entry == nullptr || !entry->isInvalid());
    return _matches.count(entry);
}

const symtable::Entry* VarMatch::map(const symtable::Entry* const entry) const {
    assert(entry == nullptr || !entry->isInvalid());
    const symtable::Entry* result = entry;
    while (contains(result)) {
        result = _matches.at(result);
        assert(result != nullptr && !result->isArray() && !result->isInvalid());
    }
    return result;
}

void VarMatch::erase(const symtable::Entry* const entry) {
    assert(entry == nullptr || (!entry->isConst() && !entry->isInvalid()));
    for (auto it = _matches.begin(); it != _matches.end(); ) {
        if (it->second == entry) {
			if (_matches.count(entry) != 0) {
				it->second = _matches.at(entry);
			} else {
				it = _matches.erase(it);
			}
        } else {
            it++;
        }
    }
    _matches.erase(entry);
}

void VarMatch::eraseGlobal(void) {
    for (auto it = _matches.begin(); it != _matches.end(); ) {
        if (it->first->isGlobal() || it->second->isGlobal()) {
            it = _matches.erase(it);
        } else {
            it++;
        }
    }
}

void VarMatch::match(const symtable::Entry* const lhs, const symtable::Entry* const rhs) {
    assert(lhs != nullptr && rhs != nullptr);
    assert(!lhs->isConst() && !lhs->isArray() && !lhs->isInvalid());
    assert(!rhs->isArray() && !rhs->isInvalid());
    erase(lhs);
    _matches[lhs] = rhs;
}

/* varProp */

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
        t3 = new std::string(midcode->labelName()); // FIXME: memory leak
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

/* symProp */

void Optim::symProp(bool& updated) {
    std::set<symtable::FuncTable*> funcs;
    SymTable::getTable().funcs(funcs, false);
    for (auto functable : funcs) {
        // perform const propagation
        auto& midcodes = functable->_midcodes;
        for (auto it = midcodes.begin(); it < midcodes.end(); ) {
            updated = _constProp(*it) || updated;
            if (*it == nullptr) {
                it = midcodes.erase(it);
            } else {
                it++;
            }
        }
        
        // preparation for var propagation
        FlowChart flowchart(functable);
        auto& blocks = flowchart.blocks();
        std::vector<VarMatch*> matches(blocks.size());
        ReachDef reachdef(flowchart);
        for (int i = 0; i < blocks.size(); i++) {
            Defs defs;
            reachdef.getIn(defs, blocks[i]);
            matches[i] = new VarMatch(defs);
        }
        
        // perform var propagation
        for (int i = 0; i < blocks.size(); i++) {
            for (auto& midcode : blocks[i]->_midcodes) {
                updated = _varProp(midcode, *(matches[i])) || updated;
            }
        }
        
        // deinit matches
        for (int i = 0; i < matches.size(); i++) {
            delete matches[i];
        }
        
        flowchart.commit();
	}
}
