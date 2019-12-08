/**********************************************
    > File Name: RegPool.cpp
    > Author: Luting Wang
    > Mail: 2457348692@qq.com 
    > Created Time: Sat Nov  9 00:00:58 2019
 **********************************************/

#include <algorithm>
#include <cassert>
#include <map>
#include <set>
#include <stack>
#include "datastream.h"
#include "midcode.h"
#include "symtable.h"

#include "../include/memory.h"

#include "Action.h"
#include "Simulator.h"

#include "../include/RegPool.h"

/* APool */

APool::APool(const symtable::FuncTable* const functable, const StackFrame& stackframe) :
    _stackframe(stackframe) {
    auto& args = functable->argList();
    for (int i = 0; i < args.size() && i < reg::a.size(); i++) {
        _regs.push_back(args[i]);
    }
}

bool APool::contains(const symtable::Entry* const entry) const {
    return std::find(_regs.begin(), _regs.end(), entry) != _regs.end();
}

Reg APool::at(const symtable::Entry* const entry) const {
    int ind = std::find(_regs.begin(), _regs.end(), entry) - _regs.begin();
    return reg::a[ind];
}

void APool::backup() const {
    for (int i = 0; i < _regs.size(); i++) {
        _stackframe.storeReg(reg::a[i]);
    }
}

void APool::restore(void) const {
    for (int i = 0; i < _regs.size(); i++) {
        _stackframe.loadReg(reg::a[i]);
    }
}

/* SPool */

class ConflictGraph {
    using Node = const symtable::Entry;
    using Graph = std::map<Node*, std::set<Node*>>;
    
    Graph _graph;
public:
    ConflictGraph(const FlowChart&, const std::vector<Node*> blackList);
    
private:
    static void _removeNode(Graph&, Node* const);
public:
    void color(std::map<Node*, Reg>&) const;
};

ConflictGraph::ConflictGraph(const FlowChart& flowchart, const std::vector<Node*> blackList) {
    const LiveVar livevar(flowchart);
    for (auto basicblock : flowchart.blocks()) {
        std::vector<Vars> varsList;
        livevar.backProp(varsList, basicblock);
        for (auto& vars : varsList) {
            for (auto entry : vars) {
                _graph[entry].insert(vars.begin(), vars.end());
            }
        }
    }
    
    // erase self loop
    for (auto& pair : _graph) {
        pair.second.erase(pair.first);
    }
    
    // erase blackList
    for (auto& node : blackList) {
        if (_graph.count(node) == 0) { continue; }
        _removeNode(_graph, node);
    }
}

void ConflictGraph::_removeNode(Graph& graph, Node* const node) {
    for (auto neighbour : graph.at(node)) {
        assert(graph.at(neighbour).count(node));
        graph.at(neighbour).erase(node);
    }
    graph.erase(node);
}

void ConflictGraph::color(std::map<Node*, Reg>& output) const {
    assert(output.empty());
    const int K = reg::s.size();
    auto graph = _graph; // `_graph` cannot be changed
    
    // find nodes to be colored
    std::stack<Node*> nodes;
    while (graph.size() > 0) {
        Node* candidate = nullptr;
        for (auto& pair : graph) {
            if (pair.second.size() >= K) { continue; }
            candidate = pair.first;
            break;
        }
        
        if (candidate == nullptr) {
            int maxDegree = K - 1;
            for (auto& pair : graph) {
                if (pair.second.size() <= maxDegree) { continue; }
                maxDegree = pair.second.size();
                candidate = pair.first;
            }
        } else {
            nodes.push(candidate);
        }
        _removeNode(graph, candidate);
    }
    
    // color `nodes`
    for (Node* head; !nodes.empty(); nodes.pop()) {
        head = nodes.top();
        std::set<Reg> preserved;
        for (auto conflictedNode : _graph.at(head)) {
            if (output.count(conflictedNode) == 0) { continue; }
            preserved.insert(output.at(conflictedNode));
        }
        for (auto reg : reg::s) {
            if (preserved.count(reg) != 0) { continue; }
            output[head] = reg;
            break;
        }
    }
}

SPool::SPool(const symtable::FuncTable* const functable, const StackFrame& stackframe) :
    _stackframe(stackframe) {
    const FlowChart flowchart(functable);
    const ConflictGraph conflictGraph(flowchart, functable->argList());
    conflictGraph.color(_regs);
}

bool SPool::contains(const symtable::Entry* const entry) const {
    return _regs.count(entry);
}

Reg SPool::at(const symtable::Entry* const entry) const {
    return _regs.at(entry);
}

void SPool::_usage(std::set<Reg>& usage) const {
    assert(usage.empty());
    for (auto& pair : _regs) {
        usage.insert(pair.second);
    }
}

void SPool::backup(void) const {
    std::set<Reg> usage;
    _usage(usage);
    for (auto reg : usage) {
        assert(std::find(reg::s.begin(), reg::s.end(), reg) != reg::s.end());
        _stackframe.storeReg(reg);
    }
}

void SPool::restore(void) const {
    std::set<Reg> usage;
    _usage(usage);
    for (auto reg : usage) {
        assert(std::find(reg::s.begin(), reg::s.end(), reg) != reg::s.end());
        _stackframe.loadReg(reg);
    }
}

/* RegPool */

RegPool::RegPool(const symtable::FuncTable* const functable, const StackFrame& stackframe) :
	_reg_a(functable, stackframe), _reg_s(functable, stackframe), _stackframe(stackframe) {}

void RegPool::genPrologue(void) const { _reg_s.backup(); }
void RegPool::genEpilogue(void) const { _reg_s.restore(); }

void RegPool::stash(void) const {
    _reg_a.backup();
    _stackframe.storeReg(Reg::ra);
}

void RegPool::unstash(void) const {
    _stackframe.loadReg(Reg::ra);
    _reg_a.restore();
}

void RegPool::simulate(const std::vector<const symtable::Entry*>& _seq,
		const std::vector<bool>& write,
		const std::vector<bool>& mask) {
    assert(_seq.size() == write.size() && _seq.size() == mask.size());
    assert(_actionCache.empty());
    ActionGen output = [this](const Reg reg, const symtable::Entry* const load, const symtable::Entry* const store)
            { this->_actionCache.push(new Action(reg, load, store)); };
    Simulator simu(output, _reg_a, _reg_s, _seq);
    for (int i = 0; i < _seq.size(); i++) {
        simu.request(write[i], mask[i]);
    }
	simu.clear();
}

void RegPool::_execute(void) {
	const Action* const action = _actionCache.front();
    _actionCache.pop();
    
    // store first!
    if (action->store != nullptr) {
        _stackframe.storeSym(action->reg, action->store);
	}
    if (action->load != nullptr) {
        _stackframe.loadSym(action->reg, action->load);
	}
    delete action;
}

Reg RegPool::request(void) {
    const Reg result = _actionCache.front()->reg;
	_execute();
    return result;
}

void RegPool::clear(void) {
	while (!_actionCache.empty()) {
        assert(_actionCache.front()->load == nullptr);
        assert(_actionCache.front()->store != nullptr);
		_execute();
	}
}
