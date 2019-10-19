/**********************************************
    > File Name: Expr.h
    > Author: Luting Wang
    > Mail: 2457348692@qq.com 
    > Created Time: Sat Sep 28 10:42:45 2019
 **********************************************/

#ifndef EXPR_H
#define EXPR_H

// For `factor`, `item`, and `expr`, the return
// value indicates whether the non-terminal is 
// integral.
//
// Note that the return value in `integer` has
// different meanings. Check the source code for 
// further information.
class Expr {
	static bool factor(void);
	static bool item(void);
public:
	static bool integer(int&);
	static bool expr(void);
};

#endif /* EXPR_H */
