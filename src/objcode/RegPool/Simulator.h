/**********************************************
    > File Name: Simulator.h
    > Author: Luting Wang
    > Mail: 2457348692@qq.com 
    > Created Time: Sun Nov 10 12:51:30 2019
 **********************************************/

#ifndef SIMULATOR_H
#define SIMULATOR_H

#include <set>
#include <vector>

#include "../include/Reg.h"

namespace symtable {
	class Entry;
}

struct Action;

class Simulator {
	const std::vector<symtable::Entry*>& _reg_a;
	const std::vector<symtable::Entry*>& _reg_s;
	std::vector<symtable::Entry*> _reg_t;
	std::vector<bool> _dirty;

	const std::vector<symtable::Entry*> _seq;
	int _counter = 0;

	// The first part of `_actions` consists of operations
	// need to be done at each request, while the second
	// part stores all the registers back to the stack, so
	// that the basic block can exit safely.
	std::vector<Action>& _actions;
public:
	Simulator(const std::vector<symtable::Entry*>& reg_a, 
			const std::vector<symtable::Entry*>& reg_s, 
			const std::vector<symtable::Entry*>& _seq, 
			std::vector<Action>& actions);

	void request(bool write, bool mask);

	void clear(void);
};

#endif /* SIMULATOR_H */
