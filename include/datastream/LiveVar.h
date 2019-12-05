/**********************************************
    > File Name: LiveVar.h
    > Author: Luting Wang
    > Mail: 2457348692@qq.com 
    > Created Time: Thu Dec  5 10:02:06 2019
 **********************************************/

#ifndef LIVE_VAR_H
#define LIVE_VAR_H

#include <set>
#include <vector>

class MidCode;
class BasicBlock;

namespace symtable {
	class Entry;
	class FuncTable;
}

class LiveVar {
	// in and out live variables for each midcode
	std::vector<std::set<const symtable::Entry*>> _in;
	std::vector<std::set<const symtable::Entry*>> _out;
public:
	LiveVar(const symtable::FuncTable* const);

	static void use(std::vector<const symtable::Entry*>&, const MidCode* const);
	static void def(symtable::Entry const*&, const MidCode* const);
	static void use(std::set<const symtable::Entry*>&, const BasicBlock* const);
	static void def(std::set<const symtable::Entry*>&, const BasicBlock* const);
};

#endif /* LIVE_VAR_H */
