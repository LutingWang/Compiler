/**********************************************
    > File Name: Expr.h
    > Author: Luting Wang
    > Mail: 2457348692@qq.com 
    > Created Time: Sat Sep 28 10:42:45 2019
 **********************************************/

#ifndef EXPR_H
#define EXPR_H

namespace symtable {
	class Entry;
}

// For `factor`, `item`, and `expr`
//     output : mid code target t0
// 
// The output of these three functions are ensured
// to be not null. Callers can determine whether
// the `expr` is int by checking `isInt` field of 
// the output.
//
// Note that the return value in `integer` has
// different meanings. Check the source code for 
// further information.
class Expr {
	static symtable::Entry* factor(void);
	static symtable::Entry* item(void);
public:
	static bool integer(int&);
	static symtable::Entry* expr(void);
};

#endif /* EXPR_H */
