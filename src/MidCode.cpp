/**********************************************
    > File Name: MidCode.cpp
    > Author: Luting Wang
    > Mail: 2457348692@qq.com 
    > Created Time: Mon Nov  4 16:21:32 2019
 **********************************************/

#include <cassert>
#include <string>
#include "compiler.h"
#include "MidCode.h"
#include "symtable.h"

std::ostream& operator << (std::ostream& output, const MidCode& midcode) {
	switch (midcode.instr) {
#ifdef CASE
	#error macro conflict
#endif
#define CASE(id, op) case MidCode::Instr::id:			\
		output << midcode.t0->name() << " = "			\
			<< midcode.t1->name() << " " #op " "		\
			<< midcode.t2->name() << std::endl;			\
		break
	CASE(ADD, +);
	case MidCode::Instr::SUB:
		output << midcode.t0->name() << " = ";
		if (midcode.t1 != nullptr) {
			output << midcode.t1->name() << ' ';
		}
		output << "- " << midcode.t2->name() << std::endl;
		break;
	case MidCode::Instr::MULT:
		output << midcode.t0->name() << " = " 
			<< midcode.t1->name() << " * " 
			<< midcode.t2->name() << std::endl;
		break;
	case MidCode::Instr::DIV:
		output << midcode.t0->name() << " = " 
			<< midcode.t1->name() << " / " 
			<< midcode.t2->name() << std::endl;
		break;
	case MidCode::Instr::LOAD_IND:
		output << midcode.t0->name() << " = " 
			<< midcode.t1->name() 
			<< '[' << midcode.t2->name() << ']' << std::endl;
		break;
	case MidCode::Instr::STORE_IND:
		output << midcode.t0->name() << '[' << midcode.t2->name() << "] = " 
			<< midcode.t1->name() << std::endl;
		break;
	case MidCode::Instr::ASSIGN:
		output << midcode.t0->name() << " = " 
			<< midcode.t1->name() << std::endl;
		break;
	case MidCode::Instr::PUSH_ARG:
		output << "push " << midcode.t1->name() << std::endl;
		break;
	case MidCode::Instr::CALL:
#if judge
		if (midcode.t0 != nullptr) {
			output << midcode.t0->name() << " = ";
		}
		output << "call " << midcode.t3 << std::endl;
#else
		output << "call " << midcode.t3 << std::endl;
		if (midcode.t0 != nullptr) {
			output << midcode.t0->name() << " = RET" << std::endl;;
		}
#endif /* judge */
		break;
	case MidCode::Instr::RET:
		output << "ret";
		if (midcode.t1 != nullptr) {
			output << ' ' << midcode.t1->name();
		}
		output << std::endl;
		break;
	case MidCode::Instr::INPUT:
		output << "input " << midcode.t0->name() << std::endl;
		break;
	case MidCode::Instr::OUTPUT:
		output << "output ";
		if (midcode.t3 != "") {
			output << '"' << midcode.t3 << "\" ";
		}
		if (midcode.t1 != nullptr) {
			output << midcode.t1->name();
		}
		output << std::endl;
		break;
	case MidCode::Instr::BGT:
		output << "if " << midcode.t1->name() << " > " << midcode.t2->name()
			<< " branch to \"" << midcode.t3 << '"' << std::endl;
		break;
	case MidCode::Instr::BGE:
		output << "if " << midcode.t1->name() << " >= " << midcode.t2->name()
			<< " branch to \"" << midcode.t3 << '"' << std::endl;
		break;
	case MidCode::Instr::BLT:
		output << "if " << midcode.t1->name() << " < " << midcode.t2->name()
			<< " branch to \"" << midcode.t3 << '"' << std::endl;
		break;
	case MidCode::Instr::BLE:
		output << "if " << midcode.t1->name() << " <= " << midcode.t2->name()
			<< " branch to \"" << midcode.t3 << '"' << std::endl;
		break;
	case MidCode::Instr::BEQ:
		output << "if " << midcode.t1->name() << " == ";
		if (midcode.t2 != nullptr) {
			output << midcode.t2->name();
		} else {
			output << "0";
		}
		output << " branch to \"" << midcode.t3 << '"' << std::endl;
		break;
	case MidCode::Instr::BNE:
		output << "if " << midcode.t1->name() << " != ";
		if (midcode.t2 != nullptr) {
			output << midcode.t2->name();
		} else {
			output << "0";
		}
		output << " branch to \"" << midcode.t3 << '"' << std::endl;
		break;
	case MidCode::Instr::GOTO:
		output << "goto " << midcode.t3 << std::endl;
		break;
	case MidCode::Instr::LABEL:
		output << midcode.t3 << ':' << std::endl;
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
	if (!table._cur->hasRet()) { table._cur->_midcode.push_back(this); }
	std::cout << *this; // TODO: replace this
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
