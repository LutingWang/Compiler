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

	std::set<symtable::Entry*> _storage;

	std::vector<ObjCode> _objcodes;

	void _compileBlock(const BasicBlock&);
public:
	ObjFunc(const std::vector<MidCode*>&, 
			const std::vector<symtable::Entry*>&);

	~ObjFunc(void);
};

#endif /* OBJ_FUNC_H */
