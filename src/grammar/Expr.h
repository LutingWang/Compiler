/**********************************************
    > File Name: Expr.h
    > Author: Luting Wang
    > Mail: 2457348692@qq.com 
    > Created Time: Sat Sep 28 10:42:45 2019
 **********************************************/

#ifndef EXPR_H
#define EXPR_H

#include "Stat.h"

class Expr {
	static void factor(void);
	static void item(void);
public:
	static int integer(void);
	static void expr(void);
	friend class Stat;
};

#endif /* EXPR_H */
