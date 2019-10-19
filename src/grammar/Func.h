/**********************************************
    > File Name: Func.h
    > Author: Luting Wang
    > Mail: 2457348692@qq.com 
    > Created Time: Sat Sep 28 10:42:17 2019
 **********************************************/

#ifndef FUNC_H
#define FUNC_H

#include "symtable.h"

class Func {
	static void args(void);
	static void def(void);
public:
	static void dec(void);
	static void argValues(const symtable::FuncTable*);
};

#endif /* FUNC_H */
