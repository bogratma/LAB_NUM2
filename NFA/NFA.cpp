//
// Created by vlad on 3/14/26.
//

#include "NFA.h"

#include <iostream>
#include <stack>

#include "../tree/Node.h"
#include "Chunk.h"
void NFA::compile(Node* root) {
    if (root==nullptr) throw std::invalid_argument("Tree is empty!");
    std::vector<Node*> nodes;
    std::stack<Chunk> base;
postOrder(root,nodes);
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
            addEpsilon(start,top.start,{},0);
            addEpsilon(start,bottom.start,{},1);
            addEpsilon(top.end,end);
            addEpsilon(bottom.end,end);
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
            addEpsilon(start, top.start);
            addEpsilon(top.end,top.start);
            addEpsilon(top.end,end);
            base.emplace(start,end);
        }
        else if (c->type==CONCAT) {
            if (base.size()<2) throw std::runtime_error("Less than 2 states!");
            const Chunk second = base.top();
            base.pop();
            Chunk first = base.top();
            base.pop();
            first.end->isAcceptable = false;
            addEpsilon(first.end, second.start);
            base.emplace(first.start,second.end);
        }
        else if (c->type == GROUP) {
            if (base.empty()) throw std::runtime_error("Base is empty!");

            Chunk inner = base.top();
            base.pop();
            int g = c->capGroup;
            State* s = create();
            State* e = create();
            inner.end->isAcceptable = false;
            e->isAcceptable = true;
            addEpsilon(s, inner.start, {{2*g}},0);
            addEpsilon(inner.end, e,   {{2*g+1}},0);
            base.emplace(s, e);
        }
        else {
            State* start = create();
            State* end = create();
            addTransition(start,end,c->name);
            end->isAcceptable=true;
            base.emplace(start,end);
        }
    }
    entry = base.top().start;
    if (base.empty()) throw std::runtime_error("Base is empty!");
    base.pop();//
}

