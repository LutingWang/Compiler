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
	static Mips __instance;
	Mips(void) {}
public:
	static const Mips& getInstance(void);

private:
	// memory regulator for global variables
	Sbss* _global = nullptr;

	// map from function name to obj codes
	std::map<std::string, ObjFunc*> _func;
public:
	~Mips(void);

	static void init(void);

	void output(void) const;
};

#endif /* MIPS_H */
