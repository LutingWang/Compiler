/**********************************************
    > File Name: compiler.h
    > Author: Luting Wang
    > Mail: 2457348692@qq.com 
    > Created Time: Mon Sep 23 15:12:22 2019
 **********************************************/

#ifndef COMPILER_H
#define COMPILER_H

#include <fstream>

#define judge 1

class InputFile;
extern InputFile input;
extern std::ofstream logger, err;

namespace symbol { 
	class Symbol; 
}

namespace lexer {
	void getsym(void);
	void traceback(const symbol::Symbol&);
}

namespace grammar {
	void parse(void);
}

#endif /* COMPILER_H */
