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
            addEps(start,top.start,{});
            addEps(start,bottom.start,{});
            addEps(top.end,end,{});
            addEps(bottom.end,end,{});
            base.emplace(start,end);
        }
        else if (c->type==STAR) {
            State* start = create();
            State* end = create();
            end->isAcceptable=true;
            addEps(start,end,{});
            if (base.empty()) throw std::runtime_error("Base is empty!");
            const Chunk top = base.top();
            top.end->isAcceptable = false;
            base.pop();
            addEps(start, top.start,{});
            addEps(top.end,top.start,{});
            addEps(top.end,end,{});
            base.emplace(start,end);
        }
        else if (c->type==CONCAT) {
            if (base.size()<2) throw std::runtime_error("Less than 2 states!");
            const Chunk second = base.top();
            base.pop();
            Chunk first = base.top();
            base.pop();
            first.end->isAcceptable = false;
            addEps(first.end,second.start,{});
            base.emplace(first.start,second.end);
        }
        else if (c->type == GROUP) {
            State* start = create();
            State* end   = create();
            end->isAcceptable = true;
            const Chunk top = base.top(); base.pop();
            top.end->isAcceptable = false;
            int reg = c->capture * 2;
            addEps(start,   top.start, {{reg}});
            addEps(top.end, end,       {{reg + 1}});
            base.emplace(start, end);
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

