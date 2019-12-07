/**********************************************
    > File Name: Translator.h
    > Author: Luting Wang
    > Mail: 2457348692@qq.com 
    > Created Time: Tue Dec  3 18:47:41 2019
 **********************************************/

#ifndef TRANSLATOR_H
#define TRANSLATOR_H

// | Operation       | Instruction       | Mips              |
// | --------------- | ----------------- | ----------------- |
// | ADD             | t0 = t1 + t2      | add t0, t1, t2    |
// | --------------- | ----------------- | ----------------- |
// | SUB             | t0 = t1 - t2      | sub t0, t1, t2    |
// | --------------- | ----------------- | ----------------- |
// | MULT            | t0 = t1 * t2      | mul t0, t1, t2    |
// | --------------- | ----------------- | ----------------- |
// | DIV             | t0 = t1 / t2      | div t1, t2        |
// |                 |                   | mflo t0           |
// | --------------- | ----------------- | ----------------- |
// | LOAD_IND        | t0 = t1[t2]       | sll t8, t2, 2     |
// |                 |                   | add t8, t8, sp/gp |
// |                 |                   | lw t0, t1(t8)     |
// | --------------- | ----------------- | ----------------- |
// | STORE_IND       | t0[t2] = t1       | sll t8, t2, 2     |
// |                 |                   | add t8, t8, sp/gp |
// |                 |                   | sw t1, t0(t8)     |
// | --------------- | ----------------- | ----------------- |
// | ASSIGN          | t0 = t1           | move t0, t1       |
// | --------------- | ----------------- | ----------------- |
// | PUSH_ARG & CALL | { push t1 }       | enumerated later  |
// |                 | [t0 =] call t3    |                   |
// | --------------- | ----------------- | ----------------- |
// | RET             | ret [t1]          | [move v0, t1]     |
// |                 |                   | epilogue          |
// |                 |                   | jr ra             |
// | --------------- | ----------------- | ----------------- |
// | INPUT           | scanf(t0)         | li v0, 5/12       |
// |                 |                   | syscall           |
// |                 |                   | move t0, v0       |
// | --------------- | ----------------- | ----------------- |
// | OUTPUT_STR      | printf(t3)        | move t8, a0       |
// |                 |                   | la a0, t3         |
// |                 |                   | li v0, 4          |
// |                 |                   | syscall           |
// |                 |                   | move a0, t8       |
// | --------------- | ----------------- | ----------------- |
// | OUTPUT_INT      | printf(t1)        | move t8, a0       |
// |                 |                   | move a0, t1       |
// |                 |                   | li v0, 1          |
// |                 |                   | syscall           |
// |                 |                   | move a0, t8       |
// | --------------- | ----------------- | ----------------- |
// | OUTPUT_CHAR     | printf(t1)        | move t8, a0       |
// |                 |                   | move a0, t1       |
// |                 |                   | li v0, 11         |
// |                 |                   | syscall           |
// |                 |                   | move a0, t8       |
// | --------------- | ----------------- | ----------------- |
// | BGT             | br t3 if t1 > t2  | bgt t1, t2, t3    |
// | --------------- | ----------------- | ----------------- |
// | BGE             | br t3 if t1 >= t2 | bge t1, t2, t3    |
// | --------------- | ----------------- | ----------------- |
// | BLT             | br t3 if t1 < t2  | blt t1, t2, t3    |
// | --------------- | ----------------- | ----------------- |
// | BLE             | br t3 if t1 <= t2 | ble t1, t2, t3    |
// | --------------- | ----------------- | ----------------- |
// | BEQ             | br t3 if t1 == t2 | beq t1, t2, t3    |
// |                 | ----------------- | ----------------- |
// |                 | br t3 if t1 == 0  | beqz t1, t3       |
// | --------------- | ----------------- | ----------------- |
// | BNE             | br t3 if t1 != t2 | bne t1, t2, t3    |
// |                 | ----------------- | ----------------- |
// |                 | br t3 if t1 != 0  | bnez t1, t3       |
// | --------------- | ----------------- | ----------------- |
// | GOTO            | goto t3           | j t3              |
// | --------------- | ----------------- | ----------------- |
// | LABEL           | t3:               | t3:               |
// | --------------- | ----------------- | ----------------- |
//
// Note: call blocks differ from other blocks. Normally,
// a call block would be translated into objcodes in the
// following form
//
//     ```
//     sw a#, #(sp)
//     sw ra, #(sp)
//     move a#, t1
//     [sw t1, #(sp)]
//     jal t3
//     lw ra, #(sp)
//     lw a#, #(sp)
//     [move t0, v0]
//     ```
//
// In English, the process would be
//
//     1. store a0~a3
//     2. store ra
//     3. load a0~a3 with args for the called func
//     4. store the rest args in stack
//     5. jump to the called func
//     6. load ra
//     7. load a0~a3
//     8. (optional) retrieve retval from v0
//
// Unfortunately, the architecture of regpool made it a
// little different for step 3. To illustrate this,
// consider the moment right after loading a0, in step 3.
// In other words, a0 is now the first arg for the called
// func, while a1~a3 are still args of the calling func.
// Now, assume that the second arg of the called func (to
// be writen into a1) is the first arg of the calling func
// (old value in a0). Since regpool has not been updated,
// it will return a0 as the src reg, which is out-dated.
//
// Fixes of the aforementioned bug are various and
// constantly changing due to reconstruction. Check the
// implementation of `_compileCallBlock` for further info.

#include <vector>
#include <string>
#include "midcode.h"
#include "symtable.h"

#include "../include/ObjCode.h"
#include "../include/RegPool.h"
#include "../include/memory.h"

class Translator {
    CodeGen& _output;
    RegPool& _regpool;
    const StackFrame& _stackframe;
public:
    Translator(CodeGen&, RegPool&, const StackFrame&);
    
private:
    // Translate midcodes but `push` and `call`.
    void _compileCode(const MidCode&);
    
    // Call blocks need to be translated as a whole.
    void _compileCallBlock(const BasicBlock&);
public:
    void compile(const BasicBlock&);
};

#endif /* TRANSLATOR_H */
