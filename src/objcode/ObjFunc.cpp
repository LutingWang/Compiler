/**********************************************
    > File Name: ObjFunc.cpp
    > Author: Luting Wang
    > Mail: 2457348692@qq.com 
    > Created Time: Mon Nov 11 20:55:23 2019
 **********************************************/

#include <cassert>
#include <fstream>
#include <set>
#include <string>
#include "midcode.h"
#include "symtable.h"
#include "Mips.h"

#include "./include/ObjCode.h"
#include "./include/RegPool.h"
#include "./include/StackFrame.h"

#include "./include/ObjFunc.h"

// | Operation  | Instruction       | Mips              |
// | ---------- | ----------------- | ----------------- |
// | ADD        | t0 = t1 + t2      | add t0, t1, t2    |
// | ---------- | ----------------- | ----------------- |
// | SUB        | t0 = t1 - t2      | sub t0, t1, t2    |
// | ---------- | ----------------- | ----------------- |
// | MULT       | t0 = t1 * t2      | mul t0, t1, t2    |
// | ---------- | ----------------- | ----------------- |
// | DIV        | t0 = t1 / t2      | div t0, t1, t2    |
// | ---------- | ----------------- | ----------------- |
// | LOAD_IND   | t0 = t1[t2]       | sll t8, t2, 2     |
// |            |                   | add t8, t8, sp    |
// |            |                   | lw t0, t1(t8)     |
// | ---------- | ----------------- | ----------------- |
// | STORE_IND  | t0[t2] = t1       | sll t8, t2, 2     |
// |            |                   | add t8, t8, sp    |
// |            |                   | sw t1, t0(t8)     |
// | ---------- | ----------------- | ----------------- |
// | ASSIGN     | t0 = t1           | move t0, t1       |
// | ---------- | ----------------- | ----------------- |
// | PUSH_ARG   | { push t1 }       | sw a#, #(sp)      |
// | &          | [t0 =] call t3    | sw ra, #(sp)      |
// | CALL       |                   | move a#, t1       |
// |            |                   | [sw t1, #(sp)]    |
// |            |                   | jal t3            |
// |            |                   | lw ra, #(sp)      |
// |            |                   | lw a#, #(sp)      |
// |            |                   | [move t0, v0]     |
// | ---------- | ----------------- | ----------------- |
// | RET        | ret [t1]          | [move v0, t1]     |
// |            |                   | epilogue          |
// |            |                   | jr ra             |
// | ---------- | ----------------- | ----------------- |
// | INPUT      | scanf(t0)         | li v0, 5          |
// |            |                   | syscall           |
// |            |                   | move t0, v0       |
// | ---------- | ----------------- | ----------------- |
// | OUTPUT     | printf(t3, t1)    | move t8, a0       |
// |            |                   | la a0, t3         |
// |            |                   | li v0, 4          |
// |            |                   | syscall           |
// |            |                   | move a0, t1       |
// |            |                   | li v0, 1          |
// |            |                   | syscall           |
// |            |                   | move a0, t8       |
// |            | ----------------- | ----------------- |
// |            | printf(t3)        | move t8, a0       |
// |            |                   | la a0, t3         |
// |            |                   | li v0, 4          |
// |            |                   | syscall           |
// |            |                   | move a0, t8       |
// |            | ----------------- | ----------------- |
// |            | printf(t1)        | move t8, a0       |
// |            |                   | move a0, t1       |
// |            |                   | li v0, 1          |
// |            |                   | syscall           |
// |            |                   | move a0, t8       |
// | ---------- | ----------------- | ----------------- |
// | BGT        | br t3 if t1 > t2  | bgt t1, t2, t3    |
// | ---------- | ----------------- | ----------------- |
// | BGE        | br t3 if t1 >= t2 | bge t1, t2, t3    |
// | ---------- | ----------------- | ----------------- |
// | BLT        | br t3 if t1 < t2  | blt t1, t2, t3    |
// | ---------- | ----------------- | ----------------- |
// | BLE        | br t3 if t1 <= t2 | ble t1, t2, t3    |
// | ---------- | ----------------- | ----------------- |
// | BEQ        | br t3 if t1 == t2 | beq t1, t2, t3    |
// |            | ----------------- | ----------------- |
// |            | br t3 if t1 == 0  | beqz t1, t3       |
// | ---------- | ----------------- | ----------------- |
// | BNE        | br t3 if t1 != t2 | bne t1, t2, t3    |
// |            | ----------------- | ----------------- |
// |            | br t3 if t1 != 0  | bnez t1, t3       |
// | ---------- | ----------------- | ----------------- |
// | GOTO       | goto t3           | j t3              |
// | ---------- | ----------------- | ----------------- |
// | LABEL      | t3:               | t3:               |
// | ---------- | ----------------- | ----------------- |

#define CASE(id) case MidCode::Instr::id
namespace {
    void requiredSyms(std::vector<symtable::Entry*>& _seq,
            std::set<symtable::Entry*>& arrays,
            std::vector<bool>& write,
            std::vector<bool>& mask,
            const MidCode& midcode) {
        switch (midcode.instr) {
#define PUSH(t, w, m) \
            _seq.push_back(midcode.t); \
            write.push_back(w); \
            mask.push_back(m)

        CASE(ADD): CASE(SUB): CASE(MULT): CASE(DIV):
            PUSH(t1, false, false);
            PUSH(t2, false, true);
            PUSH(t0, true, false);
            break;
        CASE(LOAD_IND):
            PUSH(t2, false, false);
            PUSH(t0, true, false);
            arrays.insert(midcode.t1);
            break;
        CASE(STORE_IND):
            PUSH(t2, false, false);
            PUSH(t1, false, false);
            arrays.insert(midcode.t0);
            break;
        CASE(ASSIGN):
            PUSH(t1, false, false);
            PUSH(t0, true, false);
            break;
        CASE(PUSH_ARG):
            PUSH(t1, false, false);
            break;
        CASE(CALL):
            if (midcode.t0 != nullptr) { PUSH(t0, true, false); }
            break;
        CASE(RET):
            if (midcode.t1 != nullptr) { PUSH(t1, false, false); }
            break;
        CASE(INPUT):
            PUSH(t0, true, false);
            break;
        CASE(OUTPUT):
            if (midcode.t1 != nullptr) { PUSH(t1, false, false); }
            break;
        CASE(BGT): CASE(BGE): CASE(BLT): CASE(BLE):
            PUSH(t1, false, false);
            PUSH(t2, false, true);
            break;
        CASE(BEQ): CASE(BNE):
            PUSH(t1, false, false);
            if (midcode.t2 != nullptr) { PUSH(t2, false, true); }
            break;
        default: break;
#undef PUSH
        }
    }
}

void ObjFunc::_compileBlock(const BasicBlock& basicblock) {
    const Reg noreg = Reg::zero;
    const int noimm = 0;
    const std::string nolab = "";
    Reg t0, t1, t2;
#define GEN(id, t0, t1, t2, imm, lab) \
    _objcodes.emplace_back(ObjCode::Instr::id, t0, t1, t2, imm, lab);
#define REQ _regpool->request(*_stackframe)
    if (basicblock.isFuncCall()) {
        for (Reg a : reg::a) {
            _stackframe->store(a);
        }
        _stackframe->store(Reg::ra);
        
        int argNum = basicblock.midcodes.size() - 1;
        for (int i = 0; i < argNum && i < 4; i++) {
            t1 = REQ;
            GEN(move, reg::a[i], t1, noreg, noimm, nolab);
        }
        for (int i = 4; i < argNum; i++) {
            t1 = REQ;
            GEN(sw, t1, Reg::sp, noreg, (i - argNum) * 4, nolab);
        }
        
        GEN(jal, noreg, noreg, noreg, noimm, basicblock.midcodes.back()->t3);
        
        _stackframe->load(Reg::ra);
        for (Reg a : reg::a) {
            _stackframe->load(a);
        }
        
        if (basicblock.midcodes.back()->t0 != nullptr) {
            t0 = REQ;
            GEN(move, t0, Reg::v0, noreg, noimm, nolab);
        }
    } else for (auto& midcode : basicblock.midcodes){
        switch (midcode->instr) {
        CASE(ADD):
            t1 = REQ;
            t2 = REQ;
            t0 = REQ;
            GEN(add, t0, t1, t2, noimm, nolab);
            break;
        CASE(SUB):
            t1 = REQ;
            t2 = REQ;
            t0 = REQ;
            GEN(sub, t0, t1, t2, noimm, nolab);
            break;
        CASE(MULT):
            t1 = REQ;
            t2 = REQ;
            t0 = REQ;
            GEN(mul, t0, t1, t2, noimm, nolab);
            break;
        CASE(DIV):
            t1 = REQ;
            t2 = REQ;
            t0 = REQ;
            GEN(div, t0, t1, t2, noimm, nolab);
            break;
        CASE(LOAD_IND):
            t2 = REQ;
            GEN(sll, Reg::t8, t2, noreg, 2, nolab);
            GEN(add, Reg::t8, Reg::t8, Reg::sp, noimm, nolab);
            t0 = REQ;
            GEN(lw, t0, Reg::t8, noreg, (*_stackframe)[midcode->t1], nolab);
            break;
        CASE(STORE_IND):
            t2 = REQ;
            GEN(sll, Reg::t8, t2, noreg, 2, nolab);
            GEN(add, Reg::t8, Reg::t8, Reg::sp, noimm, nolab);
            t1 = REQ;
            GEN(sw, t1, Reg::t8, noreg, (*_stackframe)[midcode->t0], nolab);
            break;
        CASE(ASSIGN):
            t1 = REQ;
            t0 = REQ;
            GEN(move, t0, t1, noreg, noimm, nolab);
            break;
        CASE(PUSH_ARG): CASE(CALL):
            assert(0);
        CASE(RET):
            if (midcode->t1 != nullptr) {
                t1 = REQ;
                GEN(move, Reg::v0, t1, noreg, noimm, nolab);
            }
            _regpool->clear(*_stackframe);
            GEN(addi, Reg::sp, Reg::sp, noreg, _stackframe->size(), nolab);
            GEN(jr, Reg::ra, noreg, noreg, noimm, nolab);
            return;
        CASE(INPUT):
            GEN(li, Reg::v0, noreg, noreg, 5, nolab);
            GEN(syscall, noreg, noreg, noreg, noimm, nolab);
            t0 = REQ;
            GEN(move, t0, Reg::v0, noreg, noimm, nolab);
            break;
        CASE(OUTPUT):
            GEN(move, Reg::t8, Reg::a0, noreg, noimm, nolab);
            if (midcode->t3 != "") {
                GEN(la, Reg::a0, noreg, noreg, noimm,  Mips::getInstance()._str[midcode->t3]);
                GEN(li, Reg::v0, noreg, noreg, 4, nolab);
                GEN(syscall, noreg, noreg, noreg, noimm, nolab);
            }
            if (midcode->t1 != nullptr) {
                t1 = REQ;
                GEN(move, Reg::a0, t1, noreg, noimm, nolab);
                GEN(li, Reg::v0, noreg, noreg, midcode->t1->isInt ? 1 : 11, nolab);
                GEN(syscall, noreg, noreg, noreg, noimm, nolab);
            }
            GEN(move, Reg::a0, Reg::t8, noreg, noimm, nolab);
            break;
        CASE(BGT):
            t1 = REQ;
            t2 = REQ;
            _regpool->clear(*_stackframe);
            GEN(bgt, noreg, t1, t2, noimm, midcode->t3);
            return;
        CASE(BGE):
            t1 = REQ;
            t2 = REQ;
            _regpool->clear(*_stackframe);
            GEN(bge, noreg, t1, t2, noimm, midcode->t3);
            return;
        CASE(BLT):
            t1 = REQ;
            t2 = REQ;
            _regpool->clear(*_stackframe);
            GEN(blt, noreg, t1, t2, noimm, midcode->t3);
            return;
        CASE(BLE):
            t1 = REQ;
            t2 = REQ;
            _regpool->clear(*_stackframe);
            GEN(ble, noreg, t1, t2, noimm, midcode->t3);
            return;
        CASE(BEQ):
            t1 = REQ;
            if (midcode->t2 == nullptr) {
                _regpool->clear(*_stackframe);
                GEN(beqz, noreg, t1, noreg, noimm, midcode->t3);
            } else {
                t2 = REQ;
                _regpool->clear(*_stackframe);
                GEN(beq, noreg, t1, t2, noimm, midcode->t3);
            }
            return;
        CASE(BNE):
            t1 = REQ;
            if (midcode->t2 == nullptr) {
                _regpool->clear(*_stackframe);
                GEN(bnez, noreg, t1, noreg, noimm, midcode->t3);
            } else {
                t2 = REQ;
                _regpool->clear(*_stackframe);
                GEN(bne, noreg, t1, t2, noimm, midcode->t3);
            }
            return;
        CASE(GOTO):
            _regpool->clear(*_stackframe);
            GEN(j, noreg, noreg, noreg, noimm, midcode->t3);
            return;
        CASE(LABEL):
            GEN(label, noreg, noreg, noreg, noimm, midcode->t3);
            break;
        default: assert(0);
        }
    }
#undef GEN
#undef REQ
    _regpool->clear(*_stackframe);
}
#undef CASE


ObjFunc::ObjFunc(const std::vector<MidCode*>& midcodes, 
		const std::vector<symtable::Entry*>& args) {
	// generate strings
	auto& strMap = Mips::getInstance()._str;
	for (auto& midcode : midcodes) {
		if (midcode->instr == MidCode::Instr::OUTPUT &&
				!strMap.count(midcode->t3)) {
			strMap.emplace(std::make_pair(midcode->t3, "str_" + std::to_string(strMap.size())));
		}
	}

	// initialize flow chart
	FlowChart flowchart(midcodes);

	// initialize register pool
	std::vector<symtable::Entry*> reg_a(4, nullptr);
	for (int i = 0; i < args.size() && i < 4; i++) {
		reg_a[i] = args[i];
	}
    _regpool = new RegPool(midcodes, reg_a);

	// simulate temporary register allocation
	for (auto& basicblock : flowchart.blocks) {
		std::vector<symtable::Entry*> _seq;
		std::vector<bool> write;
		std::vector<bool> mask;
		for (auto& midcode : basicblock->midcodes) {
			requiredSyms(_seq, _storage, write, mask, *midcode);
		}
		_regpool->simulate(_seq, write, mask);
	}

	// initialize stack frame
	_regpool->storage(_storage);
    for (auto& entry : args) {
        _storage.erase(entry);
    }
    _stackframe = new StackFrame(_objcodes, args, _storage);

    // prologue
	_objcodes.emplace_back(ObjCode::Instr::subi, Reg::sp, Reg::sp, Reg::zero, _stackframe->size(), "");
    
    // jump the last empty block
    for (auto& basicblock : flowchart.blocks) {
        _compileBlock(*basicblock);
    }
}

ObjFunc::~ObjFunc(void) {
	delete _regpool;
	delete _stackframe;
}

void ObjFunc::output(void) const {
	for (auto& objcode : _objcodes) {
		objcode.output();
	}
}

