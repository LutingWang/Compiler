/**********************************************
    > File Name: debug.h
    > Author: Luting Wang
    > Mail: 2457348692@qq.com 
    > Created Time: Sun Sep 29 21:59:58 2019
 **********************************************/

#ifndef DEBUG_H
#define DEBUG_H

#include <iostream>
#include <fstream>
#include <string>
#include "lexer.h"

#include <vector>

enum Nonterminal {
	PROGRAM, CONST_DEC, CONST_DEF, STR, INT, UNSIGNED_INT, VAR_DEC, VAR_DEF,
	FUNC_DEF_WITH_RET, FUNC_DEF_WITHOUT_RET, FUNC_CALL_WITH_RET, FUNC_CALL_WITHOUT_RET, 
	MAIN_FUNC, ARGS, VALUES, DEC_HEAD, 
	BLOCK, STAT_SERIES, STAT, ASSIGN, READ, WRITE, RET,
	COND, ITER, CONDITION, STEP, 
	EXPR, ITEM, FACTOR 
};

extern std::vector<std::string> v;
std::string print(Nonterminal);
std::string print(void);

class Debugger {
	bool _isFile;
	std::ofstream _output;
	std::ostream& output(void) { return _isFile ? _output : std::cout; }
public:
	Debugger() : _isFile(false) {}
	Debugger(std::string path) : _isFile(true), _output(path) {}
	~Debugger() { if (_isFile) { _output.close(); } }
};

#endif /* DEBUG_H */
