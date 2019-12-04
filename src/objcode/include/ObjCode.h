/**********************************************
    > File Name: ObjCode.h
    > Author: Luting Wang
    > Mail: 2457348692@qq.com 
    > Created Time: Mon Nov 11 17:06:52 2019
 **********************************************/

#ifndef OBJ_CODE_H
#define OBJ_CODE_H

#include <functional>
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
		add, sub, 
		mul, div, mflo,

		addi, subi, 
		lw, sw, 

		bgt, bge, 
		blt, ble, 
		beq, bne, 
		beqz, bnez,

		jal, j, jr,

		la, li, move, sll,

		syscall,

		label
	};
    
    static const Reg noreg;
    static const int noimm;
    static const std::string nolab;

private:
	const Instr _instr;
	const Reg _t0;
	const Reg _t1;
	const Reg _t2;
	const int _imm;
	const std::string _label;
public:
	ObjCode(const Instr, 
			const Reg, 
			const Reg, 
			const Reg, 
			const int, 
			const std::string&);

	void output(void) const;
};

std::ostream& operator << (std::ostream&, const ObjCode::Instr);

using CodeGen = std::function<void(const ObjCode::Instr, const Reg, const Reg, const Reg, const int, const std::string&)>;

#endif /* OBJ_CODE_H */
