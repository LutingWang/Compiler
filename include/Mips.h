/**********************************************
    > File Name: Mips.h
    > Author: Luting Wang
    > Mail: 2457348692@qq.com 
    > Created Time: Mon Nov 11 16:07:01 2019
 **********************************************/

#ifndef MIPS_H
#define MIPS_H

#include <map>
#include <string>
#include <vector>

class ObjFunc;
class Sbss;
class StackFrame;

class Mips {
	friend class ObjFunc;
	friend class StackFrame;

	// singleton design pattern
	static Mips _instance;
	Mips(void) {}
public:
	static Mips& getInstance(void);

private:
	// memory regulator for global variables
	Sbss* _global = nullptr;

	// map from string literals to their labels
	std::map<std::string, std::string> _str = {
		{ "", "str_0" },
		{ "\\n", "str_1" }
	};

	std::map<std::string, ObjFunc*> _func;
public:
	~Mips(void);

private:
	void _output(void) const;
public:
	void output(void);
};

#endif /* MIPS_H */
