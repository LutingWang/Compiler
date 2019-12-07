/**********************************************
    > File Name: LiveVar.h
    > Author: Luting Wang
    > Mail: 2457348692@qq.com 
    > Created Time: Thu Dec  5 10:02:06 2019
 **********************************************/

#ifndef LIVE_VAR_H
#define LIVE_VAR_H

#include <map>
#include <set>
#include <vector>

class MidCode;
class BasicBlock;
class FlowChart;

namespace symtable {
	class Entry;
}

#define FILTER(midcode, t) (midcode->t##IsValid() && \
    !(midcode->t()->isGlobal() || midcode->t()->isConst() || midcode->t()->isArray()))

class LiveVar {
	// live variables for each basicblock, excluding global, const, or array
	std::map<const BasicBlock*, std::set<const symtable::Entry*>> _out;
public:
	LiveVar(const FlowChart&);
    
    void backProp(std::vector<std::set<const symtable::Entry*>>&, const BasicBlock* const) const;

    // arrays are excluded
	static void use(std::vector<const symtable::Entry*>&, const MidCode* const);
	static void def(symtable::Entry const*&, const MidCode* const);
    
    // use and def are generated at the same time for basicblocks
	static void analyze(std::set<const symtable::Entry*>& use,
            std::set<const symtable::Entry*>& def, const BasicBlock* const);
};

#endif /* LIVE_VAR_H */
