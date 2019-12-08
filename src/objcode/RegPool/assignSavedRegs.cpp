/**********************************************
    > File Name: assignSavedRegs.cpp
    > Author: Luting Wang
    > Mail: 2457348692@qq.com 
    > Created Time: Wed Dec  4 21:17:26 2019
 **********************************************/

#include <algorithm>
#include <cassert>
#include <map>
#include <stack>
#include "datastream.h"
#include "midcode.h"

#include "../include/RegPool.h"

class ConflictGraph {
    using Node = const symtable::Entry;
    using Graph = std::map<Node*, std::set<Node*>>;
    
    Graph _graph;
public:
    ConflictGraph(const FlowChart&);
    
private:
    static void _removeNode(Graph&, Node* const);
public:
    void color(std::map<Node*, Reg>&) const;
};

ConflictGraph::ConflictGraph(const FlowChart& flowchart) {
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

void RegPool::assignSavedRegs(const symtable::FuncTable* const functable) {
    const FlowChart flowchart(functable);
    const ConflictGraph conflictGraph(flowchart);
    conflictGraph.color(_reg_s);
}
