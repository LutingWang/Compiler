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
#include <string>
#include "compilerConfig.h"
#include "error.h"
#include "midcode.h"
#include "symtable.h"

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
	case Instr::SUB:
	case Instr::MULT:
	case Instr::DIV:
	case Instr::LOAD_IND:
	case Instr::STORE_IND:
		assert(status == 0b1110);
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
		assert(status == 0b0111 || status == 0b0101);
		break;
	case Instr::GOTO:
	case Instr::LABEL:
		assert(status == 0b0001);
		break;
	default:
		assert(0);
	}
}

bool MidCode::is(const Instr instr) const {
	return this->instr == instr;
}

const std::string& MidCode::labelName(void) const {
	switch (instr) {
#define CASE(id) case Instr::id
	CASE(CALL): CASE(OUTPUT):
	CASE(BGT): CASE(BGE):
	CASE(BLT): CASE(BLE):
	CASE(BEQ): CASE(BNE):
	CASE(GOTO): CASE(LABEL):
		break;
#undef CASE
	default: assert(0);
	}
	return t3;
}

void MidCode::gen(const Instr instr, symtable::Entry* const t0, symtable::Entry* const t1, 
			symtable::Entry* const t2, const std::string& t3) {
	if (error::happened) { return; }
	if (table.curFunc()->hasRet()) { return; }
	if (instr == Instr::CALL && t3 == table.curFunc()->name()) {
		table.curFunc()->_inline = false;
	}
	table.curFunc()->_midcode.push_back(new MidCode(instr, t0, t1, t2, t3));
}

symtable::Entry* MidCode::genVar(const bool isInt) {
	static int counter = 1;
	return table.curFunc()->push("#" + std::to_string(counter++), false, isInt);
}

symtable::Entry* MidCode::genConst(const bool isInt, const int value) {
	std::string name = (isInt ? "int$" : "char$") + std::to_string(value);
    return table.global().contains(name) ? table.global().find(name) : table._global.push(name, true, isInt, value);
}

std::string MidCode::genLabel(void) {
	static int counter = 1;
	return "label_" + std::to_string(counter++);
}

// output functions
extern std::ofstream midcode_output;

void MidCode::print(void) const { // print this piece of mid code
	switch (instr) {
#define CASE(id, op) case MidCode::Instr::id:	\
		midcode_output << t0->name() << " = "	\
			<< t1->name() << " " #op " "		\
			<< t2->name();						\
		break
	CASE(ADD, +); CASE(SUB, -); CASE(MULT, *); CASE(DIV, /);
#undef CASE

	case MidCode::Instr::LOAD_IND:
		midcode_output << t0->name() << " = " 
			<< t1->name() 
			<< '[' << t2->name() << ']';
		break;
	case MidCode::Instr::STORE_IND:
		midcode_output << t0->name() << '[' << t2->name() << "] = " 
			<< t1->name();
		break;
	case MidCode::Instr::ASSIGN:
		midcode_output << t0->name() << " = " 
			<< t1->name();
		break;
	case MidCode::Instr::PUSH_ARG:
		midcode_output << "push " << t1->name();
		break;
	case MidCode::Instr::CALL:
#if judge
		midcode_output << "call " << t3;
		if (t0 != nullptr) {
			midcode_output << std::endl << t0->name() << " = RET";
		}
#else
		if (t0 != nullptr) {
			midcode_output << t0->name() << " = ";
		}
		midcode_output << "call " << t3;
#endif /* judge */
		break;
	case MidCode::Instr::RET:
#if judeg
		midcode_output << "ret";
#else
		midcode_output << "return";
#endif /* judge */
		if (t1 != nullptr) {
			midcode_output << ' ' << t1->name();
		}
		break;
	case MidCode::Instr::INPUT:
		midcode_output << "input " << t0->name();
		break;
	case MidCode::Instr::OUTPUT:
		midcode_output << "output ";
		if (t3 != "") {
			midcode_output << '"' << t3 << "\" ";
		}
		if (t1 != nullptr) {
			midcode_output << t1->name();
		}
		break;

#if judge
	#define CASE(id, op) case MidCode::Instr::id:		\
		midcode_output << t1->name() << " " #op " "		\
			<< t2->name() << " BNZ "			\
			<< t3;					\
		break
#else
	#define CASE(id, op) case MidCode::Instr::id:		\
		midcode_output << "if " << t1->name()			\
			<< " " #op " " << t2->name()		\
			<< " branch to \"" << t3			\
			<< '"';						\
		break
#endif /* judge */
	CASE(BGT, >); CASE(BGE, >=); CASE(BLT, <); CASE(BLE, <=);
#undef CASE

#if judge
	#define CASE(id, op) case MidCode::Instr::id:		\
		midcode_output << "if " << t1->name() << " " #op " "			\
			<< ((t2 == nullptr) ? "0" : t2->name())		\
			<< " BNZ " << t3;		\
		break
#else
	#define CASE(id, op) case MidCode::Instr::id:		\
		midcode_output << "if " << t1->name() << " " #op " "			\
			<< ((t2 == nullptr) ? "0" : t2->name())		\
			<< " branch to \"" << t3 << '"';		\
		break
#endif /* judge */
	CASE(BEQ, ==); CASE(BNE, !=);
#undef CASE

	case MidCode::Instr::GOTO:
		midcode_output << "goto " << t3;
		break;
	case MidCode::Instr::LABEL:
		midcode_output << t3 << ':';
		break;
	default:
		assert(0);
	}
	midcode_output << std::endl;
}

void MidCode::print(symtable::Entry* const entry) {
#if judge
	if (entry == nullptr) { return; }
	if (entry->isConst) {
		midcode_output << "const ";
		if (entry->isInt) {
			midcode_output << "int " << entry->name() << " = " << entry->value;
		} else {
			midcode_output << "char " << entry->name() << " = '" << (char) entry->value << '\'';
		}
	} else {
		midcode_output << "var " << (entry->isInt ? "int " : "char ") << entry->name();
		if (entry->value != -1) {
			midcode_output << '[' << entry->value << ']';
		}
	}
	midcode_output << std::endl;
#endif /* judge */
}

void MidCode::print(const symtable::FuncTable* const functable) {
	if (functable == nullptr) { return; }
#if judge
	if (functable->isVoid) { midcode_output << "void"; }
	else if (functable->isInt) { midcode_output << "int"; }
	else { midcode_output << "char"; }
#else
	midcode_output << "func";
#endif /* judge */
	midcode_output << ' ' << functable->name() << "()" << std::endl;

#if judge
	const std::vector<symtable::Entry*>& argv = functable->argList();
	for (auto& entry : argv) {
		midcode_output << "para " << (entry->isInt ? "int " : "char ")
			<< entry->name() << std::endl;
	}
    
    std::set<symtable::Entry*> syms;
    functable->syms(syms);
	for (auto& entry : syms) {
		// exclude parameters
		if (entry == nullptr) { continue; }
		if (find(argv.begin(), argv.end(), entry) != argv.end()) { continue; }
		print(entry);
	}
#endif /* judge */
	for (auto& midcode : functable->midcodes()) {
		midcode->print();
	}
	midcode_output << std::endl;
}

void MidCode::output(void) {
    std::set<symtable::Entry*> globalSyms;
    table.global().syms(globalSyms);
	for (auto& entry : globalSyms) {
		print(entry);
	}
	midcode_output << std::endl;

    std::set<const symtable::FuncTable*> funcs;
    table.funcs(funcs);
	for (auto& functable : funcs) {
		print(functable);
	}
}

