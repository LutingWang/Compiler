/**********************************************
    > File Name: ObjCode.cpp
    > Author: Luting Wang
    > Mail: 2457348692@qq.com 
    > Created Time: Mon Nov 11 17:12:08 2019
 **********************************************/

#include <cassert>
#include <fstream>

#include "./include/ObjCode.h"
using namespace objcode;

extern std::ofstream mips_output;

ObjCode::~ObjCode(void) {}

PseudoCode::PseudoCode(const bool flipped) : _flipped(flipped) {}

template<const char instr[]>
void RCode<instr>::output(void) const {
    mips_output << instr << ' ' << _t0
            << ", " << _t1 << ", " << _t2 << std::endl;
}

void PseudoRCode::_output(const std::string& instr) const {
    mips_output << instr << ' ' << _t0 << ", " << _t1 << ", " << _imm << std::endl;
}

void ICode::_output(const std::string& instr) const {
    mips_output << instr << ' ' << _t0
            << ", " << _imm << '(' << _t1 << ')' << std::endl;
}

void JCode::_output(const std::string& instr) const {
    mips_output << instr << ' ' << _label << std::endl;
}

void BCode::_output(const std::string& instr) const {
    mips_output << instr << ' ' << _t1
            << ", " << _t2 << ", " << _label << std::endl;
}

void PseudoBCode::_output(const std::string& instr, const std::string& oppo) const {
    mips_output << (_flipped ? oppo : instr) << ' ' << _t1
            << ", " << _imm << ", " << _label << std::endl;
}

namespace objcode {
    const char add[] = "add";
    const char sub[] = "sub";
    const char mul[] = "mul";
    const char div[] = "div";
    const char move[] = "move";
    const char syscall[] = "syscall";
}

template<>
void Div::output(void) const {
    mips_output << "div " << _t1 << ", " << _t2 << std::endl;
    mips_output << "mflo " << _t0 << std::endl;
}

void PseudoAdd::output(void) const { _output("add"); }

void PseudoSub::output(void) const {
    _output("sub");
    if (_flipped) { mips_output << "neg " << _t0 << ", " << _t0 << std::endl; }
}

void PseudoMul::output(void) const { _output("mul"); }

void PseudoDiv::output(void) const {
    mips_output << "li " << reg::compiler_tmp << ", " << _imm << std::endl;
    mips_output << "div ";
    if (_flipped) { mips_output << reg::compiler_tmp << ", " << _t1; }
    else { mips_output << _t1 << ", " << reg::compiler_tmp; }
    mips_output << std::endl << "mflo " << _t0 << std::endl;
}

void Lw::output(void) const { _output("lw"); }
void Sw::output(void) const { _output("sw"); }

void Sll::output(void) const {
    mips_output << "sll " << _t0 << ", " << _t1 << ", " << _imm << std::endl;
}

void J::output(void) const { _output("j"); }
void Jal::output(void) const { _output("jal"); }

void Label::output(void) const {
    mips_output << _label << ':' << std::endl;
}

void Bgt::output(void) const { _output("bgt"); }
void Bge::output(void) const { _output("bge"); }
void Blt::output(void) const { _output("blt"); }
void Ble::output(void) const { _output("ble"); }
void Beq::output(void) const { _output("beq"); }
void Bne::output(void) const { _output("bne"); }
void PseudoBgt::output(void) const { _output("bgt", "ble"); }
void PseudoBge::output(void) const { _output("bge", "blt"); }
void PseudoBlt::output(void) const { _output("blt", "bge"); }
void PseudoBle::output(void) const { _output("ble", "bgt"); }
void PseudoBeq::output(void) const { _output("beq", "beq"); }
void PseudoBne::output(void) const { _output("bne", "bne"); }

void Move::output(void) const {
    mips_output << "move " << _t0 << ", " << _t1 << std::endl;
}

void Syscall::output(void) const {
    mips_output << "syscall" << std::endl;
}

void Li::output(void) const {
    mips_output << "li " << _t0 << ", " << _imm << std::endl;
}

void Jr::output(void) const {
    mips_output << "jr " << Reg::ra << std::endl;
}

void Beqz::output(void) const {
    mips_output << "beqz " << _t1 << ", " << _t2 << ", " << _label << std::endl;
}

void Bnez::output(void) const {
    mips_output << "bnez " << _t1 << ", " << _t2 << ", " << _label << std::endl;
}

void La::output(void) const {
    mips_output << "la " << _t1 << ", " << _label << std::endl;
}

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
