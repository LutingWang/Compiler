/**********************************************
    > File Name: Action.h
    > Author: Luting Wang
    > Mail: 2457348692@qq.com 
    > Created Time: Sun Nov 10 12:46:52 2019
 **********************************************/

#ifndef ACTION_H
#define ACTION_H

#include "../include/Reg.h"

namespace symtable {
	class Entry;
}

// An action describes the operation upon request of a 
// temporary register. 
struct Action {
	const Reg reg;
    symtable::Entry* const load; // sym to load to reg
    symtable::Entry* const store; // sym to write back

	Action(Reg reg, symtable::Entry* load = nullptr, 
			symtable::Entry* store = nullptr) :
		reg(reg), load(load), store(store) {}
};

#endif /* ACTION_H */
