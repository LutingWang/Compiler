/**********************************************
    > File Name: Action.h
    > Author: Luting Wang
    > Mail: 2457348692@qq.com 
    > Created Time: Sun Nov 10 12:46:52 2019
 **********************************************/

#ifndef ACTION_H
#define ACTION_H

#include "symtable/Entry.h"

#include "../include/Reg.h"

// An action describes the operation upon request of a temporary register.
class Action {
public:
	const Reg reg;
    const symtable::Entry* const load; // sym to load to reg
    const symtable::Entry* const store; // sym to write back

	Action(const Reg reg, const symtable::Entry* const load, const symtable::Entry* const store) :
		reg(reg), load(load), store(store) {}
};

#endif /* ACTION_H */
