/**********************************************
    > File Name: MidCode.cpp
    > Author: Luting Wang
    > Mail: 2457348692@qq.com 
    > Created Time: Mon Nov  4 16:21:32 2019
 **********************************************/

#include <algorithm>
#include <cassert>
#include <string>
#include "compiler.h"
#include "MidCode.h"
#include "symtable.h"

std::ostream& operator << (std::ostream& output, MidCode& midcode) {
	switch (midcode.instr) {
#ifdef CASE
	#error macro conflict
#endif
#define CASE(id, op) case MidCode::Instr::id:			\
		output << midcode.t0->name() << " = "			\
			<< midcode.t1->name() << " " #op " "		\
			<< midcode.t2->name();			\
		break
	CASE(ADD, +); CASE(MULT, *); CASE(DIV, /);
#undef CASE

	case MidCode::Instr::SUB:
		output << midcode.t0->name() << " = ";
		if (midcode.t1 != nullptr) {
			output << midcode.t1->name() << ' ';
		}
		output << "- " << midcode.t2->name();
		break;
	case MidCode::Instr::LOAD_IND:
		output << midcode.t0->name() << " = " 
			<< midcode.t1->name() 
			<< '[' << midcode.t2->name() << ']';
		break;
	case MidCode::Instr::STORE_IND:
		output << midcode.t0->name() << '[' << midcode.t2->name() << "] = " 
			<< midcode.t1->name();
		break;
	case MidCode::Instr::ASSIGN:
		output << midcode.t0->name() << " = " 
			<< midcode.t1->name();
		break;
	case MidCode::Instr::PUSH_ARG:
		output << "push " << midcode.t1->name();
		break;
	case MidCode::Instr::CALL:
#if judge
		output << "call " << midcode.t3;
		if (midcode.t0 != nullptr) {
			output << midcode.t0->name() << " = RET";
		}
#else
		if (midcode.t0 != nullptr) {
			output << midcode.t0->name() << " = ";
		}
		output << "call " << midcode.t3;
#endif /* judge */
		break;
	case MidCode::Instr::RET:
		output << "ret";
		if (midcode.t1 != nullptr) {
			output << ' ' << midcode.t1->name();
		}
		break;
	case MidCode::Instr::INPUT:
		output << "input " << midcode.t0->name();
		break;
	case MidCode::Instr::OUTPUT:
		output << "output ";
		if (midcode.t3 != "") {
			output << '"' << midcode.t3 << "\" ";
		}
		if (midcode.t1 != nullptr) {
			output << midcode.t1->name();
		}
		break;

#ifdef CASE
	#error macro conflict
#endif
#if judge
	#define CASE(id, op) case MidCode::Instr::id:		\
		output << midcode.t1->name() << " " #op " "		\
			<< midcode.t2->name() << " BNZ "			\
			<< midcode.t3;					\
		break
#else
	#define CASE(id, op) case MidCode::Instr::id:		\
		output << "if " << midcode.t1->name()			\
			<< " " #op " " << midcode.t2->name()		\
			<< " branch to \"" << midcode.t3			\
			<< '"';						\
		break
#endif /* judge */
	CASE(BGT, >); CASE(BGE, >=); CASE(BLT, <); CASE(BLE, <=);
#undef CASE

#ifdef CASE
	#error macro conflict
#endif
#if judge
	#define CASE(id, op) case MidCode::Instr::id:		\
		output << "if " << midcode.t1->name() << " " #op " "			\
			<< ((midcode.t2 == nullptr) ? "0" : midcode.t2->name())		\
			<< " BNZ " << midcode.t3;		\
		break
#else
	#define CASE(id, op) case MidCode::Instr::id:		\
		output << "if " << midcode.t1->name() << " " #op " "			\
			<< ((midcode.t2 == nullptr) ? "0" : midcode.t2->name())		\
			<< " branch to \"" << midcode.t3 << '"';		\
		break
#endif /* judge */
	CASE(BEQ, ==); CASE(BNE, !=);
#undef CASE

	case MidCode::Instr::GOTO:
		output << "goto " << midcode.t3;
		break;
	case MidCode::Instr::LABEL:
		output << midcode.t3 << ':';
		break;
	default:
		assert(0);
	}
	return output;
}

MidCode::MidCode(const Instr instr, 
		symtable::Entry* const t0, 
		symtable::Entry* const t1, 
		symtable::Entry* const t2, 
		const std::string& t3) : 
	instr(instr), t0(t0), t1(t1), t2(t2), t3(t3) { 
	int status = ((t0 != nullptr) << 3) 
		+ ((t1 != nullptr) << 2) 
		+ ((t2 != nullptr) << 1) 
		+ (t3 != "");
	switch (instr) {
	case Instr::ADD:
	case Instr::MULT:
	case Instr::DIV:
	case Instr::LOAD_IND:
	case Instr::STORE_IND:
		assert(status == 0b1110);
		break;
	case Instr::SUB:
		assert(status == 0b1110 || status == 0b1010);
		break;
	case Instr::ASSIGN:
		assert(status == 0b1100);
		break;
	case Instr::PUSH_ARG:
		assert(status == 0b0100);
		break;
	case Instr::CALL:
		assert(status == 0b1001 || status == 0b0001);
		break;
	case Instr::RET:
		assert(status == 0b0100 || status == 0b0000);
		break;
	case Instr::INPUT:
		assert(status == 0b1000);
		break;
	case Instr::OUTPUT:
		assert(status == 0b0101 || status == 0b0001 || status == 0b0100);
		break;
	case Instr::BGT:
	case Instr::BGE:
	case Instr::BLT:
	case Instr::BLE:
		assert(status == 0b0111);
		break;
	case Instr::BEQ:
	case Instr::BNE:
		assert(status == 0b1110 || status == 0b1100);
		break;
	case Instr::GOTO:
	case Instr::LABEL:
		assert(status == 0b0001);
		break;
	default:
		assert(0);
	}
}

void MidCode::gen(const Instr instr, symtable::Entry* const t0, symtable::Entry* const t1, 
			symtable::Entry* const t2, const std::string& t3) {
	// TODO: if error happened, exit
	table._cur->_midcode.push_back(new MidCode(instr, t0, t1, t2, t3));
}

symtable::Entry* MidCode::genVar(const bool isInt) {
	static int counter = 100;
	return table._cur->push("$" + std::to_string(counter++), false, isInt);
}

symtable::Entry* MidCode::genConst(const bool isInt, const int value) {
	std::string name = (isInt ? "int$" : "char$") + std::to_string(value);
	symtable::Entry* entry = table._global.find(name);
	return (entry != nullptr) ? entry : table._global.push(name, true, isInt, value);
}

std::string MidCode::genLabel(void) {
	static int counter = 1;
	return "label" + std::to_string(counter++);
}

// TODO: reconstruct
void MidCode::print(std::ostream& output) {
	for (auto& e : table._global._syms) {
		if (e.second == nullptr) { continue; }
		output << *(e.second) << std::endl;
	}
	for (auto& f : table._func) {
		if (f.second == nullptr) { continue; }
		output << *(f.second) << std::endl;
		const std::vector<symtable::Entry*>& argv = f.second->argList();
		for (auto& e : argv) {
			output << "para " << (e->isInt ? "int " : "char ") 
				<< e->name() << std::endl;
		}
		for (auto& e : f.second->_syms) {
			if (e.second == nullptr) { continue; }
			if (find(argv.begin(), argv.end(), e.second) != argv.end()) { continue; }
			output << *(e.second) << std::endl;
		}
		for (auto& mc : f.second->_midcode) {
			output << *mc << std::endl;
		}
	}
	for (auto& e : table._main._syms) {
        if (e.second == nullptr) { continue; }
		output << *(e.second) << std::endl;
	}
	for (auto& mc : table._main._midcode) {
		output << *mc << std::endl;
	}
}
