/**********************************************
    > File Name: ObjCode.h
    > Author: Luting Wang
    > Mail: 2457348692@qq.com 
    > Created Time: Mon Nov 11 17:06:52 2019
 **********************************************/

#ifndef OBJ_CODE_H
#define OBJ_CODE_H

#include <string>
#include <vector>

#include "Reg.h"

namespace symtable {
	class Entry;
}

class MidCode;

class ObjCode {
public:
	enum class Instr {
		add, addi, 
		sub, subi, 
		mul, div,
		lw, sw, 

		bgt, bge, 
		blt, ble, 
		beq, beqz, 
		bne, bnez,

		jal, jr, j,

		la, li, move, sll,

		syscall,

		label
	};

	const Instr instr;
	const Reg t0;
	const Reg t1;
	const Reg t2;
	const int imm;
	const std::string label;

	ObjCode(const Instr, 
			const Reg, 
			const Reg, 
			const Reg, 
			const int, 
			const std::string&);

	void output(void) const;
};

std::ostream& operator << (std::ostream&, const ObjCode::Instr);

#endif /* OBJ_CODE_H */
