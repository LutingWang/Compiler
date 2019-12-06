/**********************************************
    > File Name: MidCode.cpp
    > Author: Luting Wang
    > Mail: 2457348692@qq.com 
    > Created Time: Mon Nov  4 16:21:32 2019
 **********************************************/

#include <algorithm>
#include <cassert>
#include <fstream>
#include <map>
#include <set>
#include <string>
#include "compilerConfig.h"
#include "error.h"
#include "symtable/Entry.h"
#include "symtable/table.h"
#include "symtable/SymTable.h"

#include "midcode/MidCode.h"

MidCode::Instr MidCode::instr(void) const {
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
    	Instr::PUSH_ARG, Instr::RET, Instr::OUTPUT_SYM,
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

// TODO: delete these two functions
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

#if !judge
extern std::ofstream midcode_output;

void MidCode::_print(void) const {
	switch (_instr) {
#define CASE(id, op) case MidCode::Instr::id:	\
		midcode_output << t0()->name() << " = "	\
			<< t1()->name() << " " #op " "		\
			<< t2()->name();						\
		break
	CASE(ADD, +); CASE(SUB, -); CASE(MULT, *); CASE(DIV, /);
#undef CASE

	case MidCode::Instr::LOAD_IND:
		midcode_output << t0()->name() << " = " 
			<< t1()->name() 
			<< '[' << t2()->name() << ']';
		break;
	case MidCode::Instr::STORE_IND:
		midcode_output << t0()->name() << '[' << t2()->name() << "] = " 
			<< t1()->name();
		break;
	case MidCode::Instr::ASSIGN:
		midcode_output << t0()->name() << " = " 
			<< t1()->name();
		break;
	case MidCode::Instr::PUSH_ARG:
		midcode_output << "push " << t1()->name();
		break;
	case MidCode::Instr::CALL:
#if judge
		midcode_output << "call " << labelName();
		if (_t0 != nullptr) {
			midcode_output << std::endl << t0()->name() << " = RET";
		}
#else
		if (_t0 != nullptr) {
			midcode_output << t0()->name() << " = ";
		}
		midcode_output << "call " << labelName();
#endif /* judge */
		break;
	case MidCode::Instr::RET:
#if judeg
		midcode_output << "ret";
#else
		midcode_output << "return";
#endif /* judge */
		if (_t1 != nullptr) {
			midcode_output << ' ' << t1()->name();
		}
		break;
	case MidCode::Instr::INPUT:
		midcode_output << "input " << t0()->name();
		break;
	case MidCode::Instr::OUTPUT_STR:
		midcode_output << "output " << '"' << labelName() << "\" ";
        break;
    case MidCode::Instr::OUTPUT_SYM:
        midcode_output << "output " << t1()->name();
        break;

#if judge
	#define CASE(id, op) case MidCode::Instr::id:		\
		midcode_output << t1()->name() << " " #op " "		\
			<< t2()->name() << " BNZ "			\
			<< labelName();					\
		break
#else
	#define CASE(id, op) case MidCode::Instr::id:		\
		midcode_output << "if " << t1()->name()			\
			<< " " #op " " << t2()->name()		\
			<< " branch to \"" << labelName()			\
			<< '"';						\
		break
#endif /* judge */
	CASE(BGT, >); CASE(BGE, >=); CASE(BLT, <); CASE(BLE, <=);
    CASE(BEQ, ==); CASE(BNE, !=);
#undef CASE

	case MidCode::Instr::GOTO:
		midcode_output << "goto " << labelName();
		break;
	case MidCode::Instr::LABEL:
		midcode_output << labelName() << ':';
		break;
	default:
		assert(0);
	}
	midcode_output << std::endl;
}

namespace {
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
            midcode->_print();
        }
        midcode_output << std::endl;
	}
}
#endif /* judge */
