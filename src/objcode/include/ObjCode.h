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

    /* RCode */

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
    RegIns(Nop, nop);
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

    template<>
    struct RCode<nop> : ObjCode {
        static int _counter;
        const int _num;
        RCode(void) : _num(++_counter) {}
        virtual void output(void) const;
    };

    /* PseudoRCode */

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

    /* ICode */

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

    /* ICode */

    template<>
    struct ICode<li> : ObjCode {
        const Reg _t0;
        const int _imm;
        
        ICode(const Reg t0, const int imm) : _t0(t0), _imm(imm) {}
        virtual void output(void) const;
    };

    /* JCode */

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

    /* BCode */

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

    /* BZCode*/

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

    RegIns(Bgtz, bgtz);
    RegIns(Bgez, bgez);
    RegIns(Bltz, bltz);
    RegIns(Blez, blez);
    RegIns(Beqz, beqz);
    RegIns(Bnez, bnez);
    RegIns(La, la);
#undef RegIns

    /* PseudoBCode */

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

    /* Factory */

    using ArithOp = const std::function<int(int, int)>;
    using CondOp = const std::function<bool(int, int)>;

    template<typename cls, typename pseudo, const ArithOp& oper>
    struct ArithFactory {
        static const ObjCode* produce(const Reg t0, const Reg t1, const Reg t2) {
            return new cls(t0, t1, t2);
        }
        
        static const ObjCode* produce(const Reg t0, const Reg t1, const int t2) {
            return new pseudo(t0, t1, t2);
        }
        
        static const ObjCode* produce(const Reg t0, const int t1, const Reg t2) {
            return new pseudo(t0, t1, t2);
        }
        
        static const ObjCode* produce(const Reg t0, const int t1, const int t2) {
            return new Li(t0, oper(t1, t2));
        }
    };

    template<typename cls, typename pseudo, typename zero, typename invzero, const CondOp& oper>
    struct BranchFactory {
        static const ObjCode* produce(const Reg t1, const Reg t2, const std::string& label) {
            if (t1 == Reg::zero && t2 == Reg::zero) { return produce(0, 0, label); }
            else if (t1 == Reg::zero) { return new invzero(t2, label); }
            else if (t2 == Reg::zero) { return new zero(t1, label); }
            return new cls(t1, t2, label);
        }
        
        static const ObjCode* produce(const Reg t1, const int t2, const std::string& label) {
            if (t1 == Reg::zero) { return produce(0, t2, label); }
            return new pseudo(t1, t2, label);
        }
        
        static const ObjCode* produce(const int t1, const Reg t2, const std::string& label) {
            if (t2 == Reg::zero) { return produce(t1, 0, label); }
            return new pseudo(t1, t2, label);
        }
        
        static const ObjCode* produce(const int t1, const int t2, const std::string& label) {
            if (oper(t1, t2)) { return new J(label); }
            else { return new Nop(); }
        }
    };

    extern const ArithOp plus, minus, times, frac;
    extern const CondOp gt, ge, lt, le, eq, ne;

    using AddFactory = ArithFactory<Add, PseudoAdd, plus>;
    using SubFactory = ArithFactory<Sub, PseudoSub, minus>;
    using MulFactory = ArithFactory<Mul, PseudoMul, times>;
    using DivFactory = ArithFactory<Div, PseudoDiv, frac>;

    using BgtFactory = BranchFactory<Bgt, PseudoBgt, Bgtz, Bltz, gt>;
    using BgeFactory = BranchFactory<Bge, PseudoBge, Bgez, Blez, ge>;
    using BltFactory = BranchFactory<Blt, PseudoBlt, Bltz, Bgtz, lt>;
    using BleFactory = BranchFactory<Ble, PseudoBle, Blez, Bgez, le>;
    using BeqFactory = BranchFactory<Beq, PseudoBeq, Beqz, Beqz, eq>;
    using BneFactory = BranchFactory<Bne, PseudoBne, Bnez, Bnez, ne>;

    using Gen = std::function<void(const ObjCode* const)>;
}

#endif /* OBJ_CODE_H */
