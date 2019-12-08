/**********************************************
    > File Name: RegPool.cpp
    > Author: Luting Wang
    > Mail: 2457348692@qq.com 
    > Created Time: Sat Nov  9 00:00:58 2019
 **********************************************/

#include <algorithm>
#include <cassert>
#include <climits>
#include <map>
#include <set>
#include <stack>
#include "datastream.h"
#include "midcode.h"
#include "symtable.h"

#include "./include/memory.h"

#include "./include/RegPool.h"

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

/* TPool */

const Reg NO_MASK = Reg::zero;

TPool::TPool(const StackFrame& stackframe) :
    _regs(reg::t.size(), nullptr),
    _dirty(reg::t.size(), false),
    _stackframe(stackframe) {}

Reg TPool::request(const symtable::Entry* const target, const bool write, const Reg mask, const std::vector<const symtable::Entry*>& seq) {
    int ind = std::find(_regs.begin(), _regs.end(), target) - _regs.begin();
    if (ind != _regs.size()) {
        if (write) { _dirty[ind] = true; }
        return reg::t[ind];
    }
    
    // try to find a nullptr in the temporary registers
    ind = std::find(_regs.begin(), _regs.end(), nullptr) - _regs.begin();
    if (ind != _regs.size()) {
        if (!write) { _stackframe.loadSym(reg::t[ind], target); }
        _regs[ind] = target;
        _dirty[ind] = write;
        return reg::t[ind];
    }
    
    // use OPT strategy
    std::vector<int> usage(_regs.size(), INT_MAX);
    int appeared = 0; // if all `Entry`s have appeared, then stop the iteration
    for (int i = 0; i < seq.size(); i++) {
        ind = std::find(_regs.begin(), _regs.end(), seq[i]) - _regs.begin();
        if (ind == _regs.size() || usage[ind] < i) { continue; }
        usage[ind] = i;
        if (++appeared == usage.size()) { break; }
    }
    
    // if `mask` is valid, disqualify the corresponding reg
    if (mask != NO_MASK) {
        ind = std::find(reg::t.begin(), reg::t.end(), mask) - reg::t.begin();
        assert(ind != reg::t.size());
        usage[ind] = 0;
    }
    
    // find the register to be used last
    ind = std::max_element(usage.begin(), usage.end()) - usage.begin();
    if (_dirty[ind]) { _stackframe.storeSym(reg::t[ind], _regs[ind]); }
    if (!write) { _stackframe.loadSym(reg::t[ind], target); }
    _regs[ind] = target;
    _dirty[ind] = write;
    return reg::t[ind];
}

void TPool::writeback(void) {
    for (int i = 0; i < _regs.size(); i++) {
        if (!_dirty[i]) {
            _regs[i] = nullptr;
            continue;
        }
        assert(_regs[i] != nullptr);
        _stackframe.storeSym(reg::t[i], _regs[i]);
        _regs[i] = nullptr;
        _dirty[i] = false;
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
	_reg_a(functable, stackframe),
    _reg_t(stackframe),
    _reg_s(functable, stackframe),
    _stackframe(stackframe),
    _maskCache(NO_MASK) {}

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

void RegPool::foresee(const std::vector<const symtable::Entry*>& seq) {
    _seq = seq;
}

Reg RegPool::request(const bool write, const bool mask) {
    assert(!write || !mask);
    const symtable::Entry* const target = _seq[0];
    _seq.erase(_seq.begin());
    
    if (_reg_a.contains(target)) {
        _maskCache = NO_MASK;
        return _reg_a.at(target);
    }
    
    if (_reg_s.contains(target)) {
        _maskCache = NO_MASK;
        return _reg_s.at(target);
    }
    
    _maskCache = _reg_t.request(target, write, mask ? _maskCache : NO_MASK, _seq);
    return _maskCache;
}

void RegPool::clear(void) {
    assert(_seq.empty());
    _reg_t.writeback();
    _maskCache = NO_MASK;
}
