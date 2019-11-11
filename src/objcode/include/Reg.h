/**********************************************
    > File Name: Reg.h
    > Author: Luting Wang
    > Mail: 2457348692@qq.com 
    > Created Time: Sun Nov 10 12:42:26 2019
 **********************************************/

#ifndef REG_H
#define REG_H

#include <vector>

enum class Reg {
	zero,
	at,
	v0, v1,
	a0, a1, a2, a3,
	t0, t1, t2, t3, t4, t5, t6, t7,
	s0, s1, s2, s3, s4, s5, s6, s7,
	t8, t9,
	gp,
	sp,
	fp,
	ra
};

namespace reg {
	extern const std::vector<Reg> a;
	extern const std::vector<Reg> t;
	extern const std::vector<Reg> s;
}

#endif /* REG_H */
