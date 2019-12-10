/**********************************************
    > File Name: ObjCode.cpp
    > Author: Luting Wang
    > Mail: 2457348692@qq.com 
    > Created Time: Mon Nov 11 17:12:08 2019
 **********************************************/

#include <cassert>
#include <fstream>

#include "./include/ObjCode.h"

namespace objcode {
#define RegIns(id) const char id[] = #id
    // RCode and PseudoRCode
    RegIns(add); RegIns(sub);
    RegIns(mul); RegIns(div);
    RegIns(move); RegIns(syscall);
    RegIns(neg); RegIns(mflo);

    // ICode
    RegIns(lw); RegIns(sw);
    RegIns(sll); RegIns(li);

    // JCode
    RegIns(j); RegIns(jal);
    RegIns(jr); RegIns(label);

    // BCode, BZCode, and pseudoBCode
    RegIns(bgt); RegIns(bge);
    RegIns(blt); RegIns(ble);
    RegIns(beq); RegIns(bne);
    RegIns(beqz); RegIns(bnez);
    RegIns(la);
}

using namespace objcode;

extern std::ofstream mips_output;

ObjCode::~ObjCode(void) {}

PseudoCode::PseudoCode(const bool flipped) : _flipped(flipped) {}

/* RCode */

template<>
void Div::output(void) const {
    mips_output << div << ' ' << _t1 << ", " << _t2 << std::endl;
    mips_output << mflo << ' ' << _t0 << std::endl;
}

void Move::output(void) const {
    mips_output << move << ' ' << _t0 << ", " << _t1 << std::endl;
}

void Syscall::output(void) const {
    mips_output << syscall << std::endl;
}

/* PseudoRCode */

template<>
void PseudoSub::output(void) const {
    mips_output << sub << ' ' << _t0 << ", " << _t1 << ", " << _imm << std::endl;
    if (_flipped) { mips_output << neg << ' ' << _t0 << ", " << _t0 << std::endl; }
}

template<>
void PseudoDiv::output(void) const {
    mips_output << li << ' ' << reg::compiler_tmp << ", " << _imm << std::endl;
    mips_output << div << ' ';
    if (_flipped) { mips_output << reg::compiler_tmp << ", " << _t1; }
    else { mips_output << _t1 << ", " << reg::compiler_tmp; }
    mips_output << std::endl << mflo << ' ' << _t0 << std::endl;
}

/* ICode */

template<>
void Sll::output(void) const {
    mips_output << sll << ' ' << _t0 << ", " << _t1 << ", " << _imm << std::endl;
}

void Li::output(void) const {
    mips_output << "li " << _t0 << ", " << _imm << std::endl;
}

/* JCode */

void Jr::output(void) const {
    mips_output << "jr " << Reg::ra << std::endl;
}

template<>
void Label::output(void) const {
    mips_output << _label << ':' << std::endl;
}

/* Factory */

const ObjCode* ArithFactory::produce(const Reg t0, const int t1, const int t2) const {
    return new Li(t0, _op(t1, t2));
}

const ObjCode* BranchFactory::produce(const int t1, const int t2, const std::string& label) const {
    static int counter = 0;
    if (_op(t1, t2)) { return new J(label); }
    counter++;
    return new Label("invalid_branch_" + std::to_string(counter));
}

#define RegisterArithFactory(name, op) \
struct name##Factory : ArithFactory { \
    name##Factory(void) : ArithFactory([](int a, int b) { return a op b; }) {} \
    virtual const ObjCode* produce(const Reg t0, const Reg t1, const Reg t2) const { \
        return new name(t0, t1, t2); \
    } \
    virtual const ObjCode* produce(const Reg t0, const Reg t1, const int t2) const { \
        return new Pseudo##name(t0, t1, t2); \
    } \
    virtual const ObjCode* produce(const Reg t0, const int t1, const Reg t2) const { \
        return new Pseudo##name(t0, t1, t2); \
    } \
};

RegisterArithFactory(Add, +)
RegisterArithFactory(Sub, -)
RegisterArithFactory(Mul, *)
RegisterArithFactory(Div, /)

namespace objcode {
    const ArithFactory
        *addFactory = new AddFactory(),
        *subFactory = new SubFactory(),
        *mulFactory = new MulFactory(),
        *divFactory = new DivFactory();
}

// TODO: use generics and use bgtz, etc.

#define RegisterBranchFactory(name, op) \
struct name##Factory : BranchFactory { \
    name##Factory(void) : BranchFactory([](int a, int b) { return a op b; }) {} \
    virtual const ObjCode* produce(const Reg t1, const Reg t2, const std::string& label) const { \
        return new name(t1, t2, label); \
    } \
    virtual const ObjCode* produce(const Reg t1, const int t2, const std::string& label) const { \
        return new Pseudo##name(t1, t2, label); \
    } \
    virtual const ObjCode* produce(const int t1, const Reg t2, const std::string& label) const { \
        return new Pseudo##name(t1, t2, label); \
    } \
};

RegisterBranchFactory(Bgt, >)
RegisterBranchFactory(Bge, >=)
RegisterBranchFactory(Blt, <)
RegisterBranchFactory(Ble, <=)
RegisterBranchFactory(Beq, ==)
RegisterBranchFactory(Bne, !=)

namespace objcode {
    const BranchFactory
        *bgtFactory = new BgtFactory(),
        *bgeFactory = new BgeFactory(),
        *bltFactory = new BltFactory(),
        *bleFactory = new BleFactory(),
        *beqFactory = new BeqFactory(),
        *bneFactory = new BneFactory();
}
