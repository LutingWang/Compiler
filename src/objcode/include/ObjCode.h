/**********************************************
    > File Name: ObjCode.h
    > Author: Luting Wang
    > Mail: 2457348692@qq.com 
    > Created Time: Mon Nov 11 17:06:52 2019
 **********************************************/

#ifndef OBJ_CODE_H
#define OBJ_CODE_H

#include <functional>
#include <iostream>
#include <string>

#include "./Reg.h"

#define WORD_SIZE 4
#define LOG_WORD_SIZE 2

struct ObjCode {
	virtual void output(void) const = 0;
    
    virtual ~ObjCode(void);
};

struct PseudoCode : ObjCode {
    const bool _flipped;
    PseudoCode(const bool);
};

struct RCode : ObjCode {
    const Reg _t0;
    const Reg _t1;
    const Reg _t2;

    RCode(const Reg t0, const Reg t1, const Reg t2) :
        _t0(t0), _t1(t1), _t2(t2) {}
    void _output(const std::string&) const;
};

struct PseudoRCode : PseudoCode {
    const Reg _t0;
    const Reg _t1;
    const int _imm;

    PseudoRCode(const Reg t0, const Reg t1, const int imm) :
        PseudoCode(false), _t0(t0), _t1(t1), _imm(imm) {}
    PseudoRCode(const Reg t0, const int imm, const Reg t2) :
        PseudoCode(true), _t0(t0), _t1(t2), _imm(imm) {}
    void _output(const std::string&) const;
};

struct ICode : ObjCode {
    const Reg _t0;
    const Reg _t1;
    const int _imm;

    ICode(const Reg t0, const Reg t1, const int imm) :
        _t0(t0), _t1(t1), _imm(imm) {}
    void _output(const std::string&) const;
};

struct JCode : ObjCode {
    const std::string _label;
    
    JCode(const std::string& label) : _label(label) {}
    void _output(const std::string&) const;
};

struct BCode : ObjCode {
    const Reg _t1;
    const Reg _t2;
    const std::string _label;
    
    BCode(const Reg t1, const Reg t2, const std::string& label) :
        _t1(t1), _t2(t2), _label(label) {}
    void _output(const std::string&) const;
};

struct PseudoBCode : PseudoCode {
    const Reg _t1;
    const int _imm;
    const std::string _label;
    
    PseudoBCode(const Reg t1, const int imm, const std::string& label) :
        PseudoCode(false), _t1(t1), _imm(imm), _label(label) {}
    PseudoBCode(const int imm, const Reg t2, const std::string& label) :
        PseudoCode(true), _t1(t2), _imm(imm), _label(label) {}
    void _output(const std::string& instr, const std::string& oppo) const;
};

#define RegisterR(cls) \
struct cls : RCode { \
    cls(const Reg t0, const Reg t1, const Reg t2) : \
        RCode(t0, t1, t2) {} \
    virtual void output(void) const; \
};

RegisterR(Add)
RegisterR(Sub)
RegisterR(Mul)
RegisterR(Div)

#define RegisterPR(cls) \
struct cls : PseudoRCode { \
    cls(const Reg t0, const Reg t1, const int imm) : \
        PseudoRCode(t0, t1, imm) {} \
    cls(const Reg t0, const int imm, const Reg t2) : \
        PseudoRCode(t0, imm, t2) {} \
    virtual void output(void) const; \
};

RegisterPR(PseudoAdd)
RegisterPR(PseudoSub)
RegisterPR(PseudoMul)
RegisterPR(PseudoDiv)

#define RegisterI(cls) \
struct cls : ICode { \
    cls(const Reg t0, const Reg t1, const int imm) : \
        ICode(t0, t1, imm) {} \
    virtual void output(void) const; \
};

RegisterI(Lw)
RegisterI(Sw)
RegisterI(Sll)

#define RegisterJ(cls) \
struct cls : JCode { \
    cls(const std::string& label) : JCode(label) {} \
    virtual void output(void) const; \
};

RegisterJ(J)
RegisterJ(Jal)
RegisterJ(Label)

#define RegisterB(cls) \
struct cls : BCode { \
    cls(const Reg t1, const Reg t2, const std::string& label) : \
        BCode(t1, t2, label) {} \
    virtual void output(void) const; \
};

RegisterB(Bgt)
RegisterB(Bge)
RegisterB(Blt)
RegisterB(Ble)
RegisterB(Beq)
RegisterB(Bne)

#define RegisterPB(cls) \
struct cls : PseudoBCode { \
    cls(const Reg t1, const int t2, const std::string& label) : \
        PseudoBCode(t1, t2, label) {} \
    cls(const int t1, const Reg t2, const std::string& label) : \
        PseudoBCode(t1, t2, label) {} \
    virtual void output(void) const; \
};

RegisterPB(PseudoBgt)
RegisterPB(PseudoBge)
RegisterPB(PseudoBlt)
RegisterPB(PseudoBle)
RegisterPB(PseudoBeq)
RegisterPB(PseudoBne)

struct Move : RCode {
    Move(const Reg t0, const Reg t1) : RCode(t0, t1, reg::no_reg) {}
    virtual void output(void) const;
};

struct Syscall : RCode {
    Syscall(void) : RCode(reg::no_reg, reg::no_reg, reg::no_reg) {}
    virtual void output(void) const;
};

struct Li : ICode {
    Li(const Reg t0, const int imm) : ICode(t0, reg::no_reg, imm) {}
    virtual void output(void) const;
};

struct Jr : JCode {
    Jr(void) : JCode("") {}
    virtual void output(void) const;
};

struct Beqz : BCode {
    Beqz(const Reg t1, const std::string& label) :
        BCode(t1, reg::no_reg, label) {}
    virtual void output(void) const;
};

struct Bnez : BCode {
    Bnez(const Reg t1, const std::string& label) :
        BCode(t1, reg::no_reg, label) {}
    virtual void output(void) const;
};

struct La : PseudoBCode {
    La(const Reg t1, const std::string& label) :
        PseudoBCode(t1, 0, label) {}
    virtual void output(void) const;
};

struct ArithFactory {
    using Oper = std::function<int(int, int)>;
    const Oper _op;
    
    ArithFactory(const Oper& op) : _op(op) {}
    
    virtual const ObjCode* produce(const Reg, const Reg, const Reg) const = 0;
    virtual const ObjCode* produce(const Reg, const Reg, const int) const = 0;
    virtual const ObjCode* produce(const Reg, const int, const Reg) const = 0;
    virtual const ObjCode* produce(const Reg, const int, const int) const;
};

struct BranchFactory {
    using Oper = std::function<bool(int, int)>;
    const Oper _op;
    
    BranchFactory(const Oper& op) : _op(op) {}
    
    virtual const ObjCode* produce(const Reg, const Reg, const std::string&) const = 0;
    virtual const ObjCode* produce(const Reg, const int, const std::string&) const = 0;
    virtual const ObjCode* produce(const int, const Reg, const std::string&) const = 0;
    virtual const ObjCode* produce(const int, const int, const std::string&) const;
};

extern const ArithFactory
    *addFactory,
    *subFactory,
    *mulFactory,
    *divFactory;

extern const BranchFactory
    *bgtFactory,
    *bgeFactory,
    *bltFactory,
    *bleFactory,
    *beqFactory,
    *bneFactory;

using CodeGen = std::function<void(const ObjCode* const)>;

#endif /* OBJ_CODE_H */
