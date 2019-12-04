/**********************************************
    > File Name: ObjCode.cpp
    > Author: Luting Wang
    > Mail: 2457348692@qq.com 
    > Created Time: Mon Nov 11 17:12:08 2019
 **********************************************/

#include <cassert>
#include <fstream>

#include "./include/ObjCode.h"

const Reg ObjCode::noreg = Reg::zero;
const int ObjCode::noimm = 0;
const std::string ObjCode::nolab = "";

ObjCode::ObjCode(const Instr instr,
		const Reg t0,
		const Reg t1,
		const Reg t2,
		const int imm,
		const std::string& label) :
	_instr(instr),
	_t0(t0),
	_t1(t1),
	_t2(t2),
	_imm(imm),
	_label(label) {
        
    // Be aware that it is legal for some instructions
    // to have `imm` equal to `noimm`.
	int status = ((t0 != noreg) << 4) +
		((t1 != noreg) << 3) +
		((t2 != noreg) << 2) +
		((imm != noimm) << 1) +
		(label != nolab);
	switch (instr) {
	case Instr::add: case Instr::sub: 
	case Instr::mul: 
		assert(status == 0b11100);
		break;
	case Instr::div:
		assert(status == 0b01100);
		break;
	case Instr::mflo:
		assert(status == 0b10000);
		break;
	case Instr::addi: case Instr::subi:
	case Instr::lw: case Instr::sw:
		assert(status == 0b11010 || status == 0b11000);
		break;
	case Instr::bgt: case Instr::bge:
	case Instr::blt: case Instr::ble:
	case Instr::beq: case Instr::bne:
		assert(status == 0b01101);
		break;
	case Instr::beqz: case Instr::bnez:
		assert(status == 0b01001);
		break;
	case Instr::jal: case Instr::j: 
		assert(status == 0b00001);
		break;
	case Instr::jr:
		assert(status == 0b10000);
		break;
	case Instr::la:
		assert(status == 0b10001);
		break;
	case Instr::li:
		assert(status == 0b10010 || status == 0b10000);
		break;
	case Instr::move:
		assert(status == 0b11000);
		break;
	case Instr::sll:
		assert(status == 0b11010 || status == 0b11000);
		break;
	case Instr::syscall:
		assert(status == 0b00000);
		break;
	case Instr::label:
		assert(status == 0b00001);
		break;
	default: assert(0);
	}
}

std::ostream& operator << (std::ostream& output, const ObjCode::Instr instr) {
	switch (instr) {
#define CASE(id) case ObjCode::Instr::id: output << #id; break
	CASE(add); CASE(sub); 
	CASE(mul); CASE(div); CASE(mflo);

	CASE(addi); CASE(subi); 
	CASE(lw); CASE(sw); 

	CASE(bgt); CASE(bge); 
	CASE(blt); CASE(ble); 
	CASE(beq); CASE(bne); 
	CASE(beqz); CASE(bnez);

	CASE(jal); CASE(j); CASE(jr);

	CASE(la); CASE(li); CASE(move); CASE(sll);

	CASE(syscall);

	CASE(label);
#undef CASE
	default: assert(0);
	}
	return output;
}

extern std::ofstream mips_output;

void ObjCode::output(void) const {
	if (_instr != ObjCode::Instr::label) {
		mips_output << _instr << ' ';
	}
	switch (_instr) {
	case Instr::add: case Instr::sub: 
	case Instr::mul: 
		mips_output << _t0 << ", " << _t1 << ", " << _t2 << std::endl;
		break;
	case Instr::div:
		mips_output << _t1 << ", " << _t2 << std::endl;
		break;
	case Instr::mflo:
		mips_output << _t0 << std::endl;
		break;
	case Instr::addi: case Instr::subi:
		mips_output << _t0 << ", " << _t1 << ", " << _imm << std::endl;
		break;
	case Instr::lw: case Instr::sw:
		mips_output << _t0 << ", " << _imm << '(' << _t1 << ')' << std::endl;
		break;
	case Instr::bgt: case Instr::bge:
	case Instr::blt: case Instr::ble:
	case Instr::beq: case Instr::bne:
		mips_output << _t1 << ", " << _t2 << ", " << _label << std::endl;
		break;
	case Instr::beqz: case Instr::bnez:
		mips_output << _t1 << ", " << _label << std::endl;
		break;
	case Instr::jal: 
		mips_output << _label << std::endl;
		break;
	case Instr::j: 
		mips_output << _label << std::endl;
		break;
	case Instr::jr:
		mips_output << _t0 << std::endl;
		break;
	case Instr::la:
		mips_output << _t0 << ", " << _label << std::endl;;
		break;
	case Instr::li:
		mips_output << _t0 << ", " << _imm << std::endl;;
		break;
	case Instr::move:
		mips_output << _t0 << ", " << _t1 << std::endl;;
		break;
	case Instr::sll:
		mips_output << _t0 << ", " << _t1 << ", " << _imm << std::endl;
		break;
	case Instr::syscall:
        mips_output << std::endl;
		break;
	case Instr::label:
		mips_output << _label << ':' << std::endl;
		break;
	default: assert(0);
	}
}

