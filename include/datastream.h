/**********************************************
    > File Name: datastream.h
    > Author: Luting Wang
    > Mail: 2457348692@qq.com 
    > Created Time: Thu Dec  5 10:02:06 2019
 **********************************************/

#ifndef DATASTREAM_H
#define DATASTREAM_H

#include <map>
#include <set>
#include <vector>

class MidCode;
class BasicBlock;
class FlowChart;

namespace symtable {
	class Entry;
}

using Vars = std::set<const symtable::Entry*>;

class LiveVar {
    static void use(std::vector<const symtable::Entry*>&, const MidCode* const);
    static void def(symtable::Entry const*&, const MidCode* const);
    static void analyze(Vars& use, Vars& def, const BasicBlock* const);
    
	std::map<const BasicBlock*, Vars> _out;
public:
	LiveVar(const FlowChart&);
    
    void backProp(std::vector<Vars>&, const BasicBlock* const) const;
};

using Defs = std::set<const MidCode*>;

class ReachDef {
    static void gen(std::map<const symtable::Entry*, const MidCode*>&, const BasicBlock* const);
    static void kill(Defs&, const std::map<const symtable::Entry*, const MidCode*>& blockGen,
            const std::map<const symtable::Entry*, Defs>& funcGen);
    static void analyze(std::vector<Defs>& gen, std::vector<Defs>& kill,
            const std::vector<BasicBlock*>&);
    
    std::map<const BasicBlock*, Defs> _in;
public:
    ReachDef(const FlowChart&);
    
    void getIn(Defs&, const BasicBlock* const) const;
};

#endif /* DATASTREAM_H */
