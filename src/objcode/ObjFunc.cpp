/**********************************************
    > File Name: ObjFunc.cpp
    > Author: Luting Wang
    > Mail: 2457348692@qq.com 
    > Created Time: Mon Nov 11 20:55:23 2019
 **********************************************/

#include <algorithm>
#include <cassert>
#include <fstream>
#include <set>
#include <string>
#include "midcode/MidCode.h"
#include "midcode/BasicBlock.h"
#include "midcode/FlowChart.h"
#include "symtable/table.h"
#include "symtable/Entry.h"
#include "symtable/SymTable.h"

#include "./include/ObjCode.h"
#include "./include/RegPool.h"
#include "./include/memory.h"
#include "./include/StrPool.h"

#include "./include/ObjFunc.h"

std::map<std::string, ObjFunc*> ObjFunc::_func;

void ObjFunc::init(void) {
    std::set<const symtable::FuncTable*> funcs;
    SymTable::getTable().funcs(funcs);
    for (auto& functable : funcs) {
        _func[functable->name()] = new ObjFunc(functable);
    }
}

extern std::ofstream mips_output;

void ObjFunc::output(void) {
	for (auto& pair : _func) {
		mips_output << std::endl
			<< pair.first << ':' << std::endl;
        for (auto& objcode : pair.second->_objcodes) {
            objcode.output();
        }
	}
}

void ObjFunc::deinit(void) {
    for (auto& pair : _func) {
        delete pair.second;
    }
}

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
// |            |                   | add t8, t8, sp/gp |
// |            |                   | lw t0, t1(t8)     |
// | ---------- | ----------------- | ----------------- |
// | STORE_IND  | t0[t2] = t1       | sll t8, t2, 2     |
// |            |                   | add t8, t8, sp/gp |
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
// | OUTPUT_STR | printf(t3)        | move t8, a0       |
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

namespace {
    // TODO: remove arrays
    void requiredSyms(std::vector<const symtable::Entry*>& _seq,
            std::set<const symtable::Entry*>& arrays,
            std::vector<bool>& write,
            std::vector<bool>& mask,
            const MidCode& midcode) {
        switch (midcode.instr()) {
#define PUSH(t, w, m) \
            _seq.push_back(midcode.t()); \
            write.push_back(w); \
            mask.push_back(m)

        case MidCode::Instr::ADD:
        case MidCode::Instr::SUB:
        case MidCode::Instr::MULT:
        case MidCode::Instr::DIV:
            PUSH(t1, false, false);
            PUSH(t2, false, true);
            PUSH(t0, true, false);
            break;
        case MidCode::Instr::LOAD_IND:
            PUSH(t2, false, false);
            PUSH(t0, true, false);
            arrays.insert(midcode.t1());
            break;
        case MidCode::Instr::STORE_IND:
            PUSH(t2, false, false);
            PUSH(t1, false, false);
            arrays.insert(midcode.t0());
            break;
        case MidCode::Instr::ASSIGN:
            PUSH(t1, false, false);
            PUSH(t0, true, false);
            break;
        case MidCode::Instr::PUSH_ARG:
            PUSH(t1, false, false);
            break;
        case MidCode::Instr::CALL:
            if (midcode.t0() != nullptr) { PUSH(t0, true, false); }
            break;
        case MidCode::Instr::RET:
            if (midcode.t1() != nullptr) { PUSH(t1, false, false); }
            break;
        case MidCode::Instr::INPUT:
            PUSH(t0, true, false);
            break;
        case MidCode::Instr::OUTPUT_SYM:
            PUSH(t1, false, false);
            break;
        case MidCode::Instr::BGT:
        case MidCode::Instr::BGE:
        case MidCode::Instr::BLT:
        case MidCode::Instr::BLE:
        case MidCode::Instr::BEQ:
        case MidCode::Instr::BNE:
            PUSH(t1, false, false);
            PUSH(t2, false, true);
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
        
        int argNum = basicblock.midcodes().size() - 1;
        for (int i = 0; i < argNum && i < reg::a.size(); i++) {
            t1 = REQ;
			if (std::find(reg::a.begin(), reg::a.end(), t1) == reg::a.end()) {
				GEN(move, reg::a[i], t1, noreg, noimm, nolab);
			} else {
                GEN(lw, reg::a[i], Reg::sp, noreg, (*_stackframe)[t1], nolab);
			}
        }
        for (int i = reg::a.size(); i < argNum; i++) {
            t1 = REQ;
            if (std::find(reg::a.begin(), reg::a.end(), t1) == reg::a.end()) {
                GEN(sw, t1, Reg::sp, noreg, (i - argNum) * 4, nolab);
            } else {
                GEN(lw, Reg::t8, Reg::sp, noreg, (*_stackframe)[t1], nolab);
                GEN(sw, Reg::t8, Reg::sp, noreg, (i - argNum) * 4, nolab);
            }
        }
        
        GEN(jal, noreg, noreg, noreg, noimm, basicblock.midcodes().back()->labelName());
        
        _stackframe->load(Reg::ra);
        for (Reg a : reg::a) {
            _stackframe->load(a);
        }
        
        if (basicblock.midcodes().back()->t0() != nullptr) {
            t0 = REQ;
            GEN(move, t0, Reg::v0, noreg, noimm, nolab);
        }
    } else for (auto midcode : basicblock.midcodes()){
        switch (midcode->instr()) {
#define CASE(id, op) case MidCode::Instr::id: \
            t1 = REQ; t2 = REQ; t0 = REQ; \
            GEN(op, t0, t1, t2, noimm, nolab); \
            break
                
        CASE(ADD, add); CASE(SUB, sub); CASE(MULT, mul); CASE(DIV, div);
#undef CASE

        case MidCode::Instr::LOAD_IND:
            t2 = REQ;
            GEN(sll, Reg::t8, t2, noreg, 2, nolab);
            GEN(add, Reg::t8, Reg::t8, _stackframe->isLocal(midcode->t1()) ? Reg::sp : Reg::gp, noimm, nolab);
            t0 = REQ;
            GEN(lw, t0, Reg::t8, noreg, (*_stackframe)[midcode->t1()], nolab);
            break;
        case MidCode::Instr::STORE_IND:
            t2 = REQ;
            GEN(sll, Reg::t8, t2, noreg, 2, nolab);
            GEN(add, Reg::t8, Reg::t8, _stackframe->isLocal(midcode->t0()) ? Reg::sp : Reg::gp, noimm, nolab);
            t1 = REQ;
            GEN(sw, t1, Reg::t8, noreg, (*_stackframe)[midcode->t0()], nolab);
            break;
        case MidCode::Instr::ASSIGN:
            t1 = REQ;
            t0 = REQ;
            GEN(move, t0, t1, noreg, noimm, nolab);
            break;
        case MidCode::Instr::PUSH_ARG:
        case MidCode::Instr::CALL:
            assert(0);
        case MidCode::Instr::RET:
            if (midcode->t1() != nullptr) {
                t1 = REQ;
                GEN(move, Reg::v0, t1, noreg, noimm, nolab);
            }
            _regpool->clear(*_stackframe);
            GEN(addi, Reg::sp, Reg::sp, noreg, _stackframe->size(), nolab);
            GEN(jr, Reg::ra, noreg, noreg, noimm, nolab);
            return;
        case MidCode::Instr::INPUT:
            GEN(li, Reg::v0, noreg, noreg, midcode->t0()->isInt() ? 5 : 12, nolab);
            GEN(syscall, noreg, noreg, noreg, noimm, nolab);
            t0 = REQ;
            GEN(move, t0, Reg::v0, noreg, noimm, nolab);
            break;
        case MidCode::Instr::OUTPUT_STR:
            GEN(move, Reg::t8, Reg::a0, noreg, noimm, nolab);
            GEN(la, Reg::a0, noreg, noreg, noimm,  strpool[midcode->labelName()]);
            GEN(li, Reg::v0, noreg, noreg, 4, nolab);
            GEN(syscall, noreg, noreg, noreg, noimm, nolab);
            GEN(move, Reg::a0, Reg::t8, noreg, noimm, nolab);
            break;
        case MidCode::Instr::OUTPUT_SYM:
            t1 = REQ;
            GEN(move, Reg::a0, t1, noreg, noimm, nolab);
            GEN(li, Reg::v0, noreg, noreg, midcode->t1()->isInt() ? 1 : 11, nolab);
            GEN(syscall, noreg, noreg, noreg, noimm, nolab);
            GEN(move, Reg::a0, Reg::t8, noreg, noimm, nolab);
            break;
                
#define CASE(id, op) case MidCode::Instr::id: \
            t1 = REQ; t2 = REQ; \
            _regpool->clear(*_stackframe); \
            GEN(op, noreg, t1, t2, noimm, midcode->labelName()); \
            return
                
        CASE(BGT, bgt); CASE(BGE, bge); CASE(BLT, blt); CASE(BLE, ble);
        CASE(BEQ, beq); CASE(BNE, bne);
#undef CASE

        case MidCode::Instr::GOTO:
            _regpool->clear(*_stackframe);
            GEN(j, noreg, noreg, noreg, noimm, midcode->labelName());
            return;
        case MidCode::Instr::LABEL:
            GEN(label, noreg, noreg, noreg, noimm, midcode->labelName());
            break;
        default: assert(0);
        }
    }
#undef GEN
#undef REQ
    _regpool->clear(*_stackframe);
}

ObjFunc::ObjFunc(const symtable::FuncTable* const functable) {
	auto& args = functable->argList();
	auto& midcodes = functable->midcodes();

	// initialize flow chart
	FlowChart flowchart(functable);

	// initialize register pool
	std::vector<const symtable::Entry*> reg_a(reg::a.size(), nullptr);
	for (int i = 0; i < args.size() && i < reg::a.size(); i++) {
		reg_a[i] = args[i];
	}
    _regpool = new RegPool(midcodes, reg_a);

	// simulate temporary register allocation
	for (auto& basicblock : flowchart.blocks()) {
		std::vector<const symtable::Entry*> _seq;
		std::vector<bool> write;
		std::vector<bool> mask;
		for (auto& midcode : basicblock->midcodes()) {
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

    for (auto& basicblock : flowchart.blocks()) {
        _compileBlock(*basicblock);
    }
}

ObjFunc::~ObjFunc(void) {
	delete _regpool;
	delete _stackframe;
}
