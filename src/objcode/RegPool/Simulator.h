/**********************************************
    > File Name: Simulator.h
    > Author: Luting Wang
    > Mail: 2457348692@qq.com 
    > Created Time: Sun Nov 10 12:51:30 2019
 **********************************************/

#ifndef SIMULATOR_H
#define SIMULATOR_H

#include <functional>
#include <map>
#include <queue>
#include <set>
#include <vector>
#include "symtable.h"

#include "../include/Reg.h"

using ActionGen = std::function<void(const Reg, const symtable::Entry* const, const symtable::Entry* const)>;

class Action;

class Simulator {
	const std::vector<const symtable::Entry*>& _reg_a;
	const std::map<const symtable::Entry*, Reg>& _reg_s;
	std::vector<const symtable::Entry*> _reg_t;
	std::vector<bool> _dirty;
    
    int _maskCache;

    std::vector<const symtable::Entry*> _seq;

	ActionGen& _output;
public:
	Simulator(ActionGen&,
            const std::vector<const symtable::Entry*>& reg_a,
			const std::map<const symtable::Entry*, Reg>& reg_s, 
			const std::vector<const symtable::Entry*>& _seq);

	void request(bool write, bool mask);

	void clear(void);
};

#endif /* SIMULATOR_H */
