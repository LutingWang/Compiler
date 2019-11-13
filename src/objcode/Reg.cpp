/**********************************************
    > File Name: Reg.cpp
    > Author: Luting Wang
    > Mail: 2457348692@qq.com 
    > Created Time: Sun Nov 10 13:43:14 2019
 **********************************************/

#include <cassert>
#include <iostream>
#include <vector>

#include "./include/Reg.h"

const std::vector<Reg> reg::a = {
	Reg::a0,
	Reg::a1,
	Reg::a2,
	Reg::a3
};

const std::vector<Reg> reg::t = {
	Reg::t0,
	Reg::t1,
	Reg::t2,
	Reg::t3,
	Reg::t4,
	Reg::t5,
	Reg::t6,
	Reg::t7
};

const std::vector<Reg> reg::s = {
	Reg::s0,
	Reg::s1,
	Reg::s2,
	Reg::s3,
	Reg::s4,
	Reg::s5,
	Reg::s6,
	Reg::s7
};

std::ostream& operator << (std::ostream& output, const Reg reg) {
	switch (reg) {
#define CASE(id) case Reg::id: output << "$" #id; break
	CASE(zero); CASE(at);
	CASE(v0); CASE(v1);
	CASE(a0); CASE(a1); CASE(a2); CASE(a3);
	CASE(t0); CASE(t1); CASE(t2); CASE(t3); 
	CASE(t4); CASE(t5); CASE(t6); CASE(t7);
	CASE(s0); CASE(s1); CASE(s2); CASE(s3); 
	CASE(s4); CASE(s5); CASE(s6); CASE(s7);
	CASE(t8); CASE(t9);
	CASE(gp); CASE(sp); CASE(fp); CASE(ra);
#undef CASE
	default: assert(0);
	}
	return output;
}
