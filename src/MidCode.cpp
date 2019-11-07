/**********************************************
    > File Name: MidCode.cpp
    > Author: Luting Wang
    > Mail: 2457348692@qq.com 
    > Created Time: Mon Nov  4 16:21:32 2019
 **********************************************/

#include <algorithm>
#include <cassert>
#include <fstream>
#include <string>
#include "compilerConfig.h"
#include "error.h"
#include "symtable.h"
#include "MidCode.h"

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
	case Instr::PUSH_ARG:
		assert(status == 0b1100);
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

void MidCode::gen(const Instr instr, symtable::Entry* const t0, symtable::Entry* const t1, 
			symtable::Entry* const t2, const std::string& t3) {
	if (error::happened) { return; }
	if (table.curFunc()->hasRet()) { return; }
	if (instr == MidCode::Instr::CALL && t3 == table.curFunc()->name()) {
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
	symtable::Entry* entry = table._global.find(name);
	return (entry != nullptr) ? entry : table._global.push(name, true, isInt, value);
}

std::string MidCode::genLabel(void) {
	static int counter = 1;
	return "label#" + std::to_string(counter++);
}

// output functions
extern std::ofstream midcode_output;

void MidCode::print(void) const { // print this piece of mid code
	switch (instr) {
#ifdef CASE
	#error macro conflict
#endif /* CASE */
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
#if !judge
		midcode_output << " to " << t0->name();
#endif /* judge */
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
		midcode_output << "ret";
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

#ifdef CASE
	#error macro conflict
#endif /* CASE */
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

#ifdef CASE
	#error macro conflict
#endif /* CASE */
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

void MidCode::print(symtable::Entry* const e) {
#if judge
	if (e == nullptr) { return; }
	if (e->isConst) {
		midcode_output << "const ";
		if (e->isInt) {
			midcode_output << "int " << e->name() << " = " << e->value;
		} else {
			midcode_output << "char " << e->name() << " = '" << (char) e->value << '\'';
		}
	} else {
		midcode_output << "var " << (e->isInt ? "int " : "char ") << e->name(); 
		if (e->value != -1) {
			midcode_output << '[' << e->value << ']';
		}
	}
	midcode_output << std::endl;
#endif /* judge */
}

void MidCode::print(const symtable::FuncTable* const ft) {
	if (ft == nullptr) { return; }
#if judge
	if (ft->isVoid) { midcode_output << "void"; }
	else if (ft->isInt) { midcode_output << "int"; }
	else { midcode_output << "char"; }
#else
	midcode_output << "func";
#endif /* judge */
	midcode_output << ' ' << ft->name() << "()" << std::endl;

#if judge
	const std::vector<symtable::Entry*>& argv = ft->argList();
	for (auto& e : argv) {
		midcode_output << "para " << (e->isInt ? "int " : "char ") 
			<< e->name() << std::endl;
	}
	for (auto& e : ft->_syms) {
		// exclude parameters
		if (e.second == nullptr) { continue; }
		if (find(argv.begin(), argv.end(), e.second) != argv.end()) { continue; }
		print(e.second);
	}
#endif /* judge */
	for (auto& mc : ft->_midcode) {
		mc->print();
	}
	midcode_output << std::endl;
}

void MidCode::output(void) {
	for (auto& e : table._global._syms) { 
		print(e.second); 
	}
	midcode_output << std::endl;

	for (auto& f : table._func) { 
		print(f.second); 
	}

	print(&(table._main));
}

