/**********************************************
    > File Name: mips.cpp
    > Author: Luting Wang
    > Mail: 2457348692@qq.com 
    > Created Time: Mon Nov 11 15:40:01 2019
 **********************************************/

#include <fstream>

#include "./include/ObjFunc.h"
#include "./include/memory.h"
#include "./include/StrPool.h"

#include "mips.h"

void mips::init(void) {
	Sbss::init();
	strpool.init();
    ObjFunc::init();
}

void mips::deinit(void) {
    ObjFunc::deinit();
    Sbss::deinit();
}

extern std::ofstream mips_output;

void mips::output(void) {
	mips_output << ".data" << std::endl;
	strpool.output();

	mips_output << std::endl 
		<< ".text" << std::endl
		<< "jal main" << std::endl
		<< "li $v0, 10" << std::endl
		<< "syscall" << std::endl;

    ObjFunc::output();
}

