/**********************************************
    > File Name: ObjCode.h
    > Author: Luting Wang
    > Mail: 2457348692@qq.com 
    > Created Time: Mon Nov 11 17:06:52 2019
 **********************************************/

#ifndef OBJ_CODE_H
#define OBJ_CODE_H

#include <fstream>
#include <functional>
#include <iostream>
#include <string>

#include "./Reg.h"

#define WORD_SIZE 4
#define LOG_WORD_SIZE 2

extern std::ofstream mips_output;

namespace objcode {
    struct ObjCode {
        virtual void output(void) const = 0;
        virtual ~ObjCode(void);
    };

    struct PseudoCode : ObjCode {
        const bool _flipped;
        PseudoCode(const bool);
    };

    template<const char instr[]>
    struct RCode : ObjCode {
        const Reg _t0;
        const Reg _t1;
        const Reg _t2;

        RCode(const Reg t0, const Reg t1, const Reg t2) :
            _t0(t0), _t1(t1), _t2(t2) {}
        virtual void output(void) const {
            mips_output << instr << ' ' << _t0
                << ", " << _t1 << ", " << _t2 << std::endl;
        }
    };

#define RegIns(cls, ins) \
    extern const char ins[]; \
    using cls = RCode<ins>;

    RegIns(Add, add);
    RegIns(Sub, sub);
    RegIns(Mul, mul);
    RegIns(Div, div);
    RegIns(Move, move);
    RegIns(Syscall, syscall);
#undef RegIns

    template<>
    struct RCode<move> : ObjCode {
        const Reg _t0;
        const Reg _t1;
        
        RCode(const Reg t0, const Reg t1) : _t0(t0), _t1(t1) {}
        virtual void output(void) const;
    };

    template<>
    struct RCode<syscall> : ObjCode {
        RCode(void) {}
        virtual void output(void) const;
    };

    template<const char instr[]>
    struct PseudoRCode : PseudoCode {
        const Reg _t0;
        const Reg _t1;
        const int _imm;

        PseudoRCode(const Reg t0, const Reg t1, const int imm) :
            PseudoCode(false), _t0(t0), _t1(t1), _imm(imm) {}
        PseudoRCode(const Reg t0, const int imm, const Reg t2) :
            PseudoCode(true), _t0(t0), _t1(t2), _imm(imm) {}
        virtual void output(void) const {
            mips_output << instr << ' ' << _t0 << ", " << _t1 << ", " << _imm << std::endl;
        }
    };

#define RegIns(cls, ins) \
    extern const char ins[]; \
    using cls = PseudoRCode<ins>

    RegIns(PseudoAdd, add);
    RegIns(PseudoSub, sub);
    RegIns(PseudoMul, mul);
    RegIns(PseudoDiv, div);
#undef RegIns

    template<const char instr[]>
    struct ICode : ObjCode {
        const Reg _t0;
        const Reg _t1;
        const int _imm;

        ICode(const Reg t0, const Reg t1, const int imm) :
            _t0(t0), _t1(t1), _imm(imm) {}
        virtual void output(void) const {
            mips_output << instr << ' ' << _t0
                    << ", " << _imm << '(' << _t1 << ')' << std::endl;
        }
    };

#define RegIns(cls, ins) \
    extern const char ins[]; \
    using cls = ICode<ins>

    RegIns(Lw, lw);
    RegIns(Sw, sw);
    RegIns(Sll, sll);
    RegIns(Li, li);
#undef RegIns

    template<>
    struct ICode<li> : ObjCode {
        const Reg _t0;
        const int _imm;
        
        ICode(const Reg t0, const int imm) : _t0(t0), _imm(imm) {}
        virtual void output(void) const;
    };

    template<const char instr[]>
    struct JCode : ObjCode {
        const std::string _label;
        
        JCode(const std::string& label) : _label(label) {}
        virtual void output(void) const {
            mips_output << instr << ' ' << _label << std::endl;
        }
    };

#define RegIns(cls, ins) \
    extern const char ins[]; \
    using cls = JCode<ins>

    RegIns(J, j);
    RegIns(Jal, jal);
    RegIns(Jr, jr);
    RegIns(Label, label);
#undef RegIns

    template<>
    struct JCode<jr> : ObjCode {
        JCode(void) {}
        virtual void output(void) const;
    };

    template<const char instr[]>
    struct BCode : ObjCode {
        const Reg _t1;
        const Reg _t2;
        const std::string _label;
        
        BCode(const Reg t1, const Reg t2, const std::string& label) :
            _t1(t1), _t2(t2), _label(label) {}
        virtual void output(void) const {
            mips_output << instr << ' ' << _t1
                    << ", " << _t2 << ", " << _label << std::endl;
        }
    };

#define RegIns(cls, ins) \
    extern const char ins[]; \
    using cls = BCode<ins>

    RegIns(Bgt, bgt);
    RegIns(Bge, bge);
    RegIns(Blt, blt);
    RegIns(Ble, ble);
    RegIns(Beq, beq);
    RegIns(Bne, bne);
#undef RegIns

    template<const char instr[]>
    struct BZCode : ObjCode {
        const Reg _t1;
        const std::string _label;
        
        BZCode(const Reg t1, const std::string& label) :
            _t1(t1), _label(label) {}
        virtual void output(void) const {
            mips_output << instr << ' ' << _t1 << ", " << _label << std::endl;
        }

    };

#define RegIns(cls, ins) \
    extern const char ins[]; \
    using cls = BZCode<ins>

    RegIns(Beqz, beqz);
    RegIns(Bnez, bnez);
    RegIns(La, la);
#undef RegIns

    template<const char instr[], const char oppo[]>
    struct PseudoBCode : PseudoCode {
        const Reg _t1;
        const int _imm;
        const std::string _label;
        
        PseudoBCode(const Reg t1, const int imm, const std::string& label) :
            PseudoCode(false), _t1(t1), _imm(imm), _label(label) {}
        PseudoBCode(const int imm, const Reg t2, const std::string& label) :
            PseudoCode(true), _t1(t2), _imm(imm), _label(label) {}
        virtual void output(void) const {
            mips_output << (_flipped ? oppo : instr) << ' ' << _t1
                    << ", " << _imm << ", " << _label << std::endl;
        }
    };

#define RegIns(cls, ins, oppo) \
    extern const char ins[]; \
    using cls = PseudoBCode<ins, oppo>;

    RegIns(PseudoBgt, bgt, ble);
    RegIns(PseudoBge, bge, blt);
    RegIns(PseudoBlt, blt, bge);
    RegIns(PseudoBle, ble, bgt);
    RegIns(PseudoBeq, beq, beq);
    RegIns(PseudoBne, bne, bne);
#undef RegIns

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
}

#endif /* OBJ_CODE_H */
