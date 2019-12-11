/**********************************************
    > File Name: midcode.cpp
    > Author: Luting Wang
    > Mail: 2457348692@qq.com 
    > Created Time: Mon Nov  4 16:21:32 2019
 **********************************************/

#include <algorithm>
#include <cassert>
#include <fstream>
#include <map>
#include <sstream>
#include "compilerConfig.h"
#include "error.h"
#include "symtable.h"

#include "midcode.h"
using Instr = MidCode::Instr;

/* MidCode */

Instr MidCode::instr(void) const {
    return _instr;
}

bool MidCode::t0IsValid(void) const {
    return _t0 != nullptr;
}

bool MidCode::t1IsValid(void) const {
    return _t1 != nullptr;
}

bool MidCode::t2IsValid(void) const {
    return _t2 != nullptr;
}

bool MidCode::labelIsValid(void) const {
    return _t3 != nullptr;
}

bool MidCode::t0IsLegal(void) const {
	static const std::set<Instr> legitimation {
		Instr::ADD, Instr::SUB, Instr::MULT, Instr::DIV,
    	Instr::LOAD_IND, Instr::STORE_IND, Instr::ASSIGN,
    	Instr::CALL, Instr::INPUT
	};
    return legitimation.count(instr());
}

bool MidCode::t1IsLegal(void) const {
	static const std::set<Instr> legitimation {
		Instr::ADD, Instr::SUB, Instr::MULT, Instr::DIV,
    	Instr::LOAD_IND, Instr::STORE_IND, Instr::ASSIGN,
        Instr::PUSH_ARG, Instr::RET,
        Instr::OUTPUT_INT, Instr::OUTPUT_CHAR,
    	Instr::BGT, Instr::BGE, Instr::BLT, Instr::BLE,
    	Instr::BEQ, Instr::BNE
	};
    return legitimation.count(instr());
}

bool MidCode::t2IsLegal(void) const {
	static const std::set<Instr> legitimation {
		Instr::ADD, Instr::SUB, Instr::MULT, Instr::DIV,
    	Instr::LOAD_IND, Instr::STORE_IND,
    	Instr::BGT, Instr::BGE, Instr::BLT, Instr::BLE,
    	Instr::BEQ, Instr::BNE
	};
    return legitimation.count(instr());
}

bool MidCode::labelIsLegal(void) const {
	static const std::set<Instr> legitimation {
		Instr::CALL, Instr::OUTPUT_STR,
    	Instr::BGT, Instr::BGE, Instr::BLT, Instr::BLE,
    	Instr::BEQ, Instr::BNE, Instr::GOTO, Instr::LABEL
	};
    return legitimation.count(instr());
}

const symtable::Entry* MidCode::t0(void) const {
    assert(t0IsLegal());
    return _t0;
}

const symtable::Entry* MidCode::t1(void) const {
    assert(t1IsLegal());
    return _t1;
}

const symtable::Entry* MidCode::t2(void) const {
    assert(t2IsLegal());
    return _t2;
}

const std::string& MidCode::labelName(void) const {
	assert(labelIsLegal());
	return *_t3;
}

MidCode::MidCode(const Instr instr, 
		const symtable::Entry* const t0,
		const symtable::Entry* const t1,
		const symtable::Entry* const t2,
		const std::string* const t3) : 
	_instr(instr), _t0(t0), _t1(t1), _t2(t2), _t3(t3) {
        assert(t0IsLegal() == (t0IsValid() || is(Instr::CALL)));
        assert(t1IsLegal() == (t1IsValid() || is(Instr::RET)));
        assert(t2IsLegal() == t2IsValid());
        assert(labelIsLegal() == labelIsValid());
}

MidCode::MidCode(const MidCode& other) :
    _instr(other._instr),
    _t0(other._t0),
    _t1(other._t1),
    _t2(other._t2),
    _t3(other.labelIsValid() ? new std::string(other.labelName()) : nullptr) {}

MidCode::~MidCode(void) {
	delete _t3;
    _t3 = nullptr;
}

bool MidCode::is(const Instr instr) const {
    return this->instr() == instr;
}

bool MidCode::isCalc(void) const {
    switch (this->instr()) {
    case Instr::ADD:
    case Instr::SUB:
    case Instr::MULT:
    case Instr::DIV:
        return true;
    default:
        return false;
    }
}

bool MidCode::isBranch(void) const {
    switch (this->instr()) {
    case Instr::BGT:
    case Instr::BGE:
    case Instr::BLT:
    case Instr::BLE:
    case Instr::BEQ:
    case Instr::BNE:
        return true;
    default:
        return false;
    }
}

void MidCode::_gen(const MidCode* const midcode) {
    if (error::happened) { return; }
    if (SymTable::getTable().curFunc().hasRet()) { return; }
    SymTable::getTable().curFunc()._midcodes.push_back(midcode);
}

void MidCode::gen(const Instr instr,
        const symtable::Entry* const t0,
        const symtable::Entry* const t1,
        const symtable::Entry* const t2) {
    _gen(new MidCode(instr, t0, t1, t2, nullptr));
}

void MidCode::gen(const Instr instr,
        const symtable::Entry* const t0,
        const symtable::Entry* const t1,
        const symtable::Entry* const t2,
        const std::string& t3) {
	_gen(new MidCode(instr, t0, t1, t2, new std::string(t3)));
}

const symtable::Entry* MidCode::genVar(const bool isInt) {
	static int counter = 1;
	return SymTable::getTable().curFunc().pushVar("#" + std::to_string(counter++), isInt);
}

const symtable::Entry* MidCode::genConst(const bool isInt, const int value) {
	const std::string name = (isInt ? "int$" : "char$") + std::to_string(value);
    return SymTable::getTable().global().contains(name) ? SymTable::getTable().global().find(name) :
		SymTable::getTable().global().pushConst(name, isInt, value);
}

std::string MidCode::genLabel(void) {
	static int counter = 1;
	return "label$" + std::to_string(counter++);
}

std::string MidCode::to_string(void) const {
    std::stringstream ss;
	switch (_instr) {
#define CASE(id, op) case Instr::id:	\
		ss << t0()->name() << " = "	\
			<< t1()->name() << " " #op " "		\
			<< t2()->name();						\
		break
	CASE(ADD, +); CASE(SUB, -); CASE(MULT, *); CASE(DIV, /);
#undef CASE

	case Instr::LOAD_IND:
		ss << t0()->name() << " = " 
			<< t1()->name() 
			<< '[' << t2()->name() << ']';
		break;
	case Instr::STORE_IND:
		ss << t0()->name() << '[' << t2()->name() << "] = " 
			<< t1()->name();
		break;
	case Instr::ASSIGN:
		ss << t0()->name() << " = " 
			<< t1()->name();
		break;
	case Instr::PUSH_ARG:
		ss << "push " << t1()->name();
		break;
	case Instr::CALL:
#if judge
		ss << "call " << labelName();
		if (_t0 != nullptr) {
			ss << std::endl << t0()->name() << " = RET";
		}
#else
		if (_t0 != nullptr) {
			ss << t0()->name() << " = ";
		}
		ss << "call " << labelName();
#endif /* judge */
		break;
	case Instr::RET:
#if judeg
		ss << "ret";
#else
		ss << "return";
#endif /* judge */
		if (_t1 != nullptr) {
			ss << ' ' << t1()->name();
		}
		break;
	case Instr::INPUT:
		ss << "input " << t0()->name();
		break;
	case Instr::OUTPUT_STR:
		ss << "output " << '"' << labelName() << "\" ";
        break;
    case Instr::OUTPUT_INT:
    case Instr::OUTPUT_CHAR:
        ss << "output " << t1()->name();
        break;

#if judge
	#define CASE(id, op) case Instr::id:		\
		ss << t1()->name() << " " #op " "		\
			<< t2()->name() << " BNZ "			\
			<< labelName();					\
		break
#else
	#define CASE(id, op) case Instr::id:		\
		ss << "if " << t1()->name()			\
			<< " " #op " " << t2()->name()		\
			<< " branch to \"" << labelName()			\
			<< '"';						\
		break
#endif /* judge */
	CASE(BGT, >); CASE(BGE, >=); CASE(BLT, <); CASE(BLE, <=);
    CASE(BEQ, ==); CASE(BNE, !=);
#undef CASE

	case Instr::GOTO:
		ss << "goto " << labelName();
		break;
	case Instr::LABEL:
		ss << labelName() << ':';
		break;
	default:
		assert(0);
	}
    return ss.str();
}

#if !judge
extern std::ofstream midcode_output;

void print(const symtable::Entry* const entry) {
#if judge
    if (entry == nullptr) { return; }
    if (entry->isConst()) {
        midcode_output << "const ";
        if (entry->isInt()) {
            midcode_output << "int " << entry->name() << " = " << entry->value();
        } else {
            midcode_output << "char " << entry->name() << " = '" << (char) entry->value() << '\'';
        }
    } else {
        midcode_output << "var " << (entry->isInt() ? "int " : "char ") << entry->name();
        if (entry->isArray()) {
            midcode_output << '[' << entry->value() << ']';
        }
    }
    midcode_output << std::endl;
#endif /* judge */
}

void print(const symtable::FuncTable* const functable) {
    if (functable == nullptr) { return; }
#if judge
    if (functable->isVoid()) { midcode_output << "void"; }
    else if (functable->isInt()) { midcode_output << "int"; }
    else { midcode_output << "char"; }
#else
    midcode_output << "func";
#endif /* judge */
    midcode_output << ' ' << functable->name() << "()" << std::endl;

#if judge
    const std::vector<const symtable::Entry*>& argv = functable->argList();
    for (auto entry : argv) {
        midcode_output << "para " << (entry->isInt() ? "int " : "char ")
            << entry->name() << std::endl;
    }
    
    std::set<const symtable::Entry*> syms;
    functable->syms(syms);
    for (auto entry : syms) {
        // exclude parameters
        if (find(argv.begin(), argv.end(), entry) != argv.end()) { continue; }
        print(entry);
    }
#endif /* judge */
}

void MidCode::output(void) {
    std::set<const symtable::Entry*> globalSyms;
    SymTable::getTable().global().syms(globalSyms);
	for (auto entry : globalSyms) {
		print(entry);
	}
	midcode_output << std::endl;

    std::set<const symtable::FuncTable*> funcs;
    SymTable::getTable().funcs(funcs);
	for (auto functable : funcs) {
		print(functable);
        for (auto midcode : functable->midcodes()) {
            midcode_output << midcode->to_string() << std::endl;
        }
        midcode_output << std::endl;
	}
}
#endif /* judge */

/* BasicBlock */

const std::vector<const MidCode*>& BasicBlock::midcodes(void) const {
    return _midcodes;
}

const std::set<BasicBlock*>& BasicBlock::prec(void) const {
    return _prec;
}

const std::set<BasicBlock*>& BasicBlock::succ(void) const {
    return _succ;
}

BasicBlock::BasicBlock(void) {}

BasicBlock::BasicBlock(const std::vector<const MidCode*>::const_iterator first,
        const std::vector<const MidCode*>::const_iterator last) :
    _midcodes(first, last) {}

BasicBlock::~BasicBlock(void) {}

void BasicBlock::_proceed(BasicBlock* const successor) {
    _succ.insert(successor);
    successor->_prec.insert(this);
}

bool BasicBlock::isFuncCall(void) const {
    return _midcodes.back()->is(Instr::CALL);
}

/* FlowChart */

const std::vector<BasicBlock*>& FlowChart::blocks(void) const {
    return _blocks;
}

void FlowChart::_init(const symtable::FuncTable* const functable) {
    auto& midcodes = functable->midcodes();
    
    // initialize <label name, label pos in midcode>
    std::map<std::string, int> labels;
    for (int i = 0; i < midcodes.size(); i++) {
        if (midcodes[i]->is(Instr::LABEL)) {
            labels[midcodes[i]->labelName()] = i;
        }
    }

    // Scan the mid code to determine pos of entrances.
    // An entrance can be one of the following
    //     - the first mid code of all
    //     - the first push or call statements in a function call block
    //     - the first statement after
    //         1. a call block
    //         2. a return statement
    //         3. a branch statement
    //         4. a goto statement
    //     - the statement jumped to from
    //         1. a branch statement
    //         2. a goto statement
    std::set<int> entrances;
    for (int i = 0; i < midcodes.size(); i++) {
        switch (midcodes[i]->instr()) {
        case Instr::PUSH_ARG:
        case Instr::CALL:
            entrances.insert(i);
            while (!midcodes[i]->is(Instr::CALL)) { i++; }
            entrances.insert(i + 1);
            break;
        case Instr::BGT:
        case Instr::BGE:
        case Instr::BLT:
        case Instr::BLE:
        case Instr::BEQ:
        case Instr::BNE:
        case Instr::GOTO:
            entrances.insert(labels[midcodes[i]->labelName()]);
            // fallthrough
        case Instr::RET:
            entrances.insert(i + 1);
            break;
        default: continue;
        }
    }
    entrances.erase(0); // the first mid code has to be entrance no matter what
    entrances.insert(midcodes.size()); // set up guard element
    assert(*(entrances.rbegin()) == midcodes.size());

    // Divide the mid codes into blocks. If a block
    // starts with a label, add it to `blockMap` to
    // enable other blocks to discover it.
    std::map<std::string, int> blockMap;
    int startIndex = 0;
    for (auto endIndex : entrances) {
        if (midcodes[startIndex]->is(Instr::LABEL)) {
            blockMap[midcodes[startIndex]->labelName()] = blocks().size();
        }
        _blocks.push_back(
                new BasicBlock(
                        midcodes.begin() + startIndex,
                        midcodes.begin() + endIndex
                )
        );
        startIndex = endIndex;
    }

    // Scan the blocks to store flow information.
    // For return statements, proceed to `_tail`.
    _tail = new BasicBlock();
    for (int i = 0; i < blocks().size(); i++) {
        const MidCode* exitcode = blocks()[i]->midcodes().back();
        switch (exitcode->instr()) {
        case Instr::RET:
            blocks()[i]->_proceed(_tail);
            break;
        case Instr::GOTO:
            assert(blockMap.find(exitcode->labelName()) != blockMap.end());
            blocks()[i]->_proceed(blocks()[blockMap[exitcode->labelName()]]);
            break;
        case Instr::BGT:
        case Instr::BGE:
        case Instr::BLT:
        case Instr::BLE:
        case Instr::BEQ:
        case Instr::BNE:
            assert(blockMap.find(exitcode->labelName()) != blockMap.end());
            blocks()[i]->_proceed(blocks()[blockMap[exitcode->labelName()]]);
            // fallthrough
        default:
            if (i + 1 < blocks().size()) {
                blocks()[i]->_proceed(blocks()[i + 1]);
            }
        }
    }
}

FlowChart::FlowChart(const symtable::FuncTable* const functable) :
    _functable(nullptr) {
    _init(functable);
}

FlowChart::FlowChart(symtable::FuncTable* const functable) :
    _functable(functable) {
    _init(functable);
}

FlowChart::~FlowChart(void) {
    for (auto block : blocks()) {
        delete block;
    }
    delete _tail;
}

void FlowChart::commit(void) {
    _functable->_midcodes.clear();
    for (auto block : blocks()) {
        _functable->_midcodes.insert(
                _functable->_midcodes.end(),
                block->midcodes().begin(),
                block->midcodes().end()
        );
    }
}
