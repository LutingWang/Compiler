/**********************************************
    > File Name: ObjFunc.h
    > Author: Luting Wang
    > Mail: 2457348692@qq.com 
    > Created Time: Mon Nov 11 20:48:01 2019
 **********************************************/

#ifndef OBJ_FUNC_H
#define OBJ_FUNC_H

#include <map>
#include <set>
#include <vector>

namespace symtable {
	class Entry;
	class FuncTable;
}

class MidCode;
class BasicBlock;

class ObjCode;
class RegPool;
class StackFrame;

class ObjFunc {
	static std::map<std::string, ObjFunc*> _func;
public:
	static void init(void);
	static void output(void);
	static void deinit(void);

private:
	RegPool* _regpool = nullptr;
	StackFrame* _stackframe = nullptr;
	std::set<const symtable::Entry*> _storage;
	std::vector<ObjCode> _objcodes;

public:
	ObjFunc(const symtable::FuncTable* const);
	~ObjFunc(void);

private:
	void _compileBlock(const BasicBlock&);
};

#endif /* OBJ_FUNC_H */
