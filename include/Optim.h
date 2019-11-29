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

class MidCode;
class BasicBlock;

namespace symtable {
    class Entry;
	class FuncTable;
}

class VarMatch;

class Optim {
	static const symtable::FuncTable* _calledFunc(const BasicBlock* const);
	static void _clean(void);
public:
	static void inlineExpan(void);
    
	static void commonExprElim(void);
    
private:
    static bool _constProp(const MidCode*&);
    static bool _varProp(const MidCode*&, VarMatch&);
public:
	static void symProp(void);
    
	static void peephole(void);
};

#endif /* OPTIM_H */
