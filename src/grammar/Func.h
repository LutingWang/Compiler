/**********************************************
    > File Name: Func.h
    > Author: Luting Wang
    > Mail: 2457348692@qq.com 
    > Created Time: Sat Sep 28 10:42:17 2019
 **********************************************/

#ifndef FUNC_H
#define FUNC_H

namespace symtable {
	class Entry;
	class FuncTable;
}

class Func {
	static void args(void);
	static void def(void);
public:
	static void dec(void);
	static symtable::Entry* argValues(const symtable::FuncTable* const);
};

#endif /* FUNC_H */
