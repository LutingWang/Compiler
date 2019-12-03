/**********************************************
    > File Name: ObjCode.cpp
    > Author: Luting Wang
    > Mail: 2457348692@qq.com 
    > Created Time: Mon Nov 11 17:12:08 2019
 **********************************************/

#include <cassert>
#include <fstream>

#include "./include/ObjCode.h"

ObjCode::ObjCode( const Instr instr,
		const Reg t0,
		const Reg t1,
		const Reg t2,
		const int imm,
		const std::string& label) :
	instr(instr), 
	t0(t0), 
	t1(t1), 
	t2(t2), 
	imm(imm), 
	label(label) {

	int status = ((t0 != Reg::zero) << 4) +
		((t1 != Reg::zero) << 3) + 
		((t2 != Reg::zero) << 2) +
		((imm != 0) << 1) +
		(label != "");
	switch (instr) {
#define CASE(id) case Instr::id
	CASE(add): CASE(sub): 
	CASE(mul): 
		assert(status == 0b11100);
		break;
	CASE(div):
		assert(status == 0b01100);
		break;
	CASE(mflo):
		assert(status == 0b10000);
		break;
	CASE(addi): CASE(subi):
	CASE(lw): CASE(sw):
	CASE(sll):
		assert(status == 0b11010 || status == 0b11000);
		break;
	CASE(bgt): CASE(bge):
	CASE(blt): CASE(ble):
	CASE(beq): CASE(bne):
		assert(status == 0b01101);
		break;
	CASE(beqz): CASE(bnez):
		assert(status == 0b01001);
		break;
	CASE(jal): CASE(j): CASE(label):
		assert(status == 0b00001);
		break;
	CASE(jr):
		assert(status == 0b10000);
		break;
	CASE(la):
		assert(status == 0b10001);
		break;
	CASE(li):
		assert(status == 0b10010 || status == 0b10000);
		break;
	CASE(move):
		assert(status == 0b11000);
		break;
	CASE(syscall):
		assert(status == 0b00000);
		break;
	default: assert(0);
#undef CASE
	}
}

std::ostream& operator << (std::ostream& output, const ObjCode::Instr instr) {
	switch (instr) {
#define CASE(id) case ObjCode::Instr::id: output << #id; break
		CASE(add); CASE(addi); CASE(sub); CASE(subi);
		CASE(mul); CASE(div); CASE(mflo);
		CASE(lw); CASE(sw);
		CASE(bgt); CASE(bge); CASE(blt); CASE(ble);
		CASE(beq); CASE(beqz); CASE(bne); CASE(bnez);
		CASE(jal); CASE(jr); CASE(j);
		CASE(la); CASE(li); CASE(move); CASE(sll);
		CASE(syscall); CASE(label);
#undef CASE
		default: assert(0);
	}
	return output;
}

extern std::ofstream mips_output;

void ObjCode::output(void) const {
	if (instr != ObjCode::Instr::label) {
		mips_output << instr << ' ';
	}
	switch (instr) {
#define CASE(id) case Instr::id
	CASE(add): CASE(sub): 
	CASE(mul): 
		mips_output << t0 << ", " << t1 << ", " << t2 << std::endl;
		break;
	CASE(div):
		mips_output << t1 << ", " << t2 << std::endl;
		break;
	CASE(mflo):
		mips_output << t0 << std::endl;
		break;
	CASE(addi): CASE(subi):
	CASE(sll):
		mips_output << t0 << ", " << t1 << ", " << imm << std::endl;
		break;
	CASE(lw): CASE(sw):
		mips_output << t0 << ", " << imm << '(' << t1 << ')' << std::endl;
		break;
	CASE(bgt): CASE(bge):
	CASE(blt): CASE(ble):
	CASE(beq): CASE(bne):
		mips_output << t1 << ", " << t2 << ", " << label << std::endl;
		break;
	CASE(beqz): CASE(bnez):
		mips_output << t1 << ", " << label << std::endl;
		break;
	CASE(jal): 
		mips_output << label << std::endl;
		break;
	CASE(j): 
		mips_output << label << std::endl;
		break;
	CASE(label):
		mips_output << label << ':' << std::endl;
		break;
	CASE(jr):
		mips_output << t0 << std::endl;
		break;
	CASE(la):
		mips_output << t0 << ", " << label << std::endl;;
		break;
	CASE(li):
		mips_output << t0 << ", " << imm << std::endl;;
		break;
	CASE(move):
		mips_output << t0 << ", " << t1 << std::endl;;
		break;
	CASE(syscall):
        mips_output << std::endl;
		break;
#undef CASE
	default: assert(0);
	}
}

