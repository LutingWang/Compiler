/**********************************************
    > File Name: Optim.h
    > Author: Luting Wang
    > Mail: 2457348692@qq.com 
    > Created Time: Wed Nov  6 22:11:33 2019
 **********************************************/

#ifndef OPTIM_H
#define OPTIM_H

#include <map>
#include <string>

class BasicBlock;

namespace symtable {
	class FuncTable;
}

class Optim {
	static const symtable::FuncTable* _calledFunc(const BasicBlock* const);
	static void _clean(void);
public:
	static void inlineExpan(void);
	static void commonExprElim(void);
	static void peephole(void);
};

#endif /* OPTIM_H */
