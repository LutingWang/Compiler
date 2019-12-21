/**********************************************
    > File Name: ObjCode.cpp
    > Author: Luting Wang
    > Mail: 2457348692@qq.com 
    > Created Time: Mon Nov 11 17:12:08 2019
 **********************************************/

#include <cassert>

#include "./include/ObjCode.h"

namespace objcode {
#define RegIns(id) const char id[] = #id
    // RCode and PseudoRCode
    RegIns(add); RegIns(sub);
    RegIns(mul); RegIns(div);
    RegIns(move); RegIns(syscall);
    RegIns(neg); RegIns(mflo); RegIns(nop);

    // ICode
    RegIns(lw); RegIns(sw);
    RegIns(sll); RegIns(li);

    // JCode
    RegIns(j); RegIns(jal); RegIns(jr);
    RegIns(label);

    // BCode, BZCode, and pseudoBCode
    RegIns(bgt); RegIns(bge);
    RegIns(blt); RegIns(ble);
    RegIns(beq); RegIns(bne);
    RegIns(bgtz); RegIns(bgez);
    RegIns(bltz); RegIns(blez);
    RegIns(beqz); RegIns(bnez);
    RegIns(la);
}

using namespace objcode;

ObjCode::~ObjCode(void) {}

PseudoCode::PseudoCode(const bool flipped) : _flipped(flipped) {}

/* RCode */

void Move::output(void) const {
    mips_output << move << ' ' << _t0 << ", " << _t1 << std::endl;
}

void Syscall::output(void) const {
    mips_output << syscall << std::endl;
}

int Nop::_counter = 0;

void Nop::output(void) const {
    mips_output << "invalid_instr_" << _num << ':' << std::endl;
}

/* ICode */

void Li::output(void) const {
    mips_output << "li " << _t0 << ", " << _imm << std::endl;
}

/* JCode */

void Jr::output(void) const {
    mips_output << "jr " << Reg::ra << std::endl;
}

/* Factory */

namespace objcode {
    const ArithOp plus = [](int a, int b) { return a + b; };
    const ArithOp minus = [](int a, int b) { return a - b; };
    const ArithOp times = [](int a, int b) { return a * b; };
    const ArithOp frac = [](int a, int b) { return a / b; };

    const CondOp gt = [](int a, int b) { return a > b; };
    const CondOp ge = [](int a, int b) { return a >= b; };
    const CondOp lt = [](int a, int b) { return a < b; };
    const CondOp le = [](int a, int b) { return a <= b; };
    const CondOp eq = [](int a, int b) { return a == b; };
    const CondOp ne = [](int a, int b) { return a != b; };
}

