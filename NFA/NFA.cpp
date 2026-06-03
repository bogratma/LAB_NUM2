//
// Created by vlad on 3/14/26.
//

#include "NFA.h"

#include <iostream>
#include <stack>
#include <unordered_set>

#include "../tree/Node.h"
#include "Chunk.h"
#include "../MDFA/MDFA.h"
#include "../Regex/Regex.h"
void NFA::compile(Node* root) {
    if (root==nullptr) throw std::invalid_argument("Tree is empty!");
    std::vector<Node*> nodes;
    std::stack<Chunk> base;
postOrder(root,nodes);
    for (auto c: nodes) {
        if (c->type==SYM) {
            alphabet.insert(c->name);
        }
    }
    for (auto c : nodes) {
        if (c->type==OR) {
            State* start = create();
            State* end = create();
            if (base.size()<2) throw std::runtime_error("Less than 2 states!");
            const Chunk top = base.top();
            base.pop();
            const Chunk bottom = base.top();
            base.pop();
            top.end->isAcceptable = false;
            bottom.end->isAcceptable = false;
            end->isAcceptable=true;
            addTransition(start,top.start,'$');
            addTransition(start,bottom.start,'$');
            addTransition(top.end,end,'$');
            addTransition(bottom.end,end,'$');
            base.emplace(start,end);
        }
        else if (c->type==STAR) {
            State* start = create();
            State* end = create();
            end->isAcceptable=true;
            addTransition(start,end,'$');
            if (base.empty()) throw std::runtime_error("Base is empty!");
            const Chunk top = base.top();
            top.end->isAcceptable = false;
            base.pop();
            addTransition(start, top.start,'$');
            addTransition(top.end,top.start,'$');
            addTransition(top.end,end,'$');
            base.emplace(start,end);
        }
        else if (c->type==CONCAT) {
            if (base.size()<2) throw std::runtime_error("Less than 2 states!");
            const Chunk second = base.top();
            base.pop();
            Chunk first = base.top();
            base.pop();
            first.end->isAcceptable = false;
            addTransition(first.end,second.start,'$');
            base.emplace(first.start,second.end);
        }
        else if (c->type==DOT) {
            State* start = create();
            State* end = create();
            end->isAcceptable=true;
            for (auto s:alphabet) {
                addTransition(start,end,s);
            }
            base.emplace(start,end);
        }
    else if (c->type == INTERSECT) {
    if (base.size() < 2) throw std::runtime_error("Less than 2 states for intersect!");
    const Chunk rightChunk = base.top(); base.pop();
    const Chunk leftChunk  = base.top(); base.pop();
    leftChunk.end->isAcceptable  = true;
    rightChunk.end->isAcceptable = true;
    NFA nfaLeft;
    nfaLeft.entry    = leftChunk.start;
    nfaLeft.alphabet = this->alphabet;
    extractSubNFA(leftChunk.start, nfaLeft);
    NFA nfaRight;
    nfaRight.entry    = rightChunk.start;
    nfaRight.alphabet = this->alphabet;
    extractSubNFA(rightChunk.start, nfaRight);
    DFA dfaLeft, dfaRight;
    dfaLeft.process(nfaLeft);
    dfaRight.process(nfaRight);
    MDFA mdfaLeft, mdfaRight;
    mdfaLeft.minimize(dfaLeft);
    mdfaRight.minimize(dfaRight);
    leftChunk.end->isAcceptable  = false;
    rightChunk.end->isAcceptable = false;
    MDFA result = MDFA::diff(mdfaLeft, mdfaRight, false);
    std::unordered_map<int, State*> idToState;
    for (const auto& [from, toMap] : result.tableTransitionMDFA) {
        if (!idToState.contains(from)) idToState[from] = create();
        for (const auto& [sym, to] : toMap) {
            if (!idToState.contains(to)) idToState[to] = create();
        }
    }
    if (!idToState.contains(result.startMDFA)) {
        idToState[result.startMDFA] = create();
    }
    for (const auto& [from, toMap] : result.tableTransitionMDFA) {
        for (const auto& [sym, to] : toMap) {
            addTransition(idToState[from], idToState[to], sym);
        }
    }
    State* chunkStart = idToState[result.startMDFA];
    State* chunkEnd   = create();
    chunkEnd->isAcceptable = false;
    for (int fId : result.finalPi) {
        if (idToState.contains(fId)) {
            addTransition(idToState[fId], chunkEnd, '$');
        }
    }
    base.emplace(chunkStart, chunkEnd);
}
        else if (c->type==SYM && !c->zapr.empty()) {
            std::vector<char> without;
            for( auto f : alphabet) {
                if (c->zapr.contains(f)) {
                    continue;
                }
                without.push_back(f);
            }
            State* start = create();
            State* end = create();
            end->isAcceptable=true;
            for (auto f : without) {
                addTransition(start,end,f);
            }
            base.emplace(start,end);
        }

        else {
            State* start = create();
            State* end = create();
            addTransition(start,end,c->name);
            end->isAcceptable=true;
            base.emplace(start,end);
        }
    }
    alphabet.extract('^');
    entry = base.top().start;
    if (base.empty()) throw std::runtime_error("Base is empty!");
    base.pop();//
}
void NFA::extractSubNFA(State* start, NFA& target) {
    std::stack<State*> stack;
    std::unordered_set<State*> visited;
    stack.push(start);
    while (!stack.empty()) {
        State* cur = stack.top(); stack.pop();
        if (visited.contains(cur)) continue;
        visited.insert(cur);
        for (auto& [key, to] : this->table) {
            if (key.first == cur) {
                target.addTransition(cur, to, key.second);
                if (!visited.contains(to)) {
                    stack.push(to);
                }
            }
        }
    }
}
