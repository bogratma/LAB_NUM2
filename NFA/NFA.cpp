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
    for (auto* n: nodes ) {
        if (n->type == SYM && n->name != '$')
            alphabet.insert(n->name);
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
        else if (c->type==EMPTY) {
            State* start = create();
            State* end = create();
            addTransition(start,end,c->name);
            end->isAcceptable=true;
            base.emplace(start,end);
        }
        else if (c->type == CHARSET) {
            std::set<char> actual;
            for (char ch : alphabet)
                if (!c->charClass.contains(ch))
                    actual.insert(ch);
            if (actual.empty()) throw std::runtime_error("Empty charset");
            State* start = create();
            State* end   = create();
            end->isAcceptable = true;
            for (char ch : actual)
                addTransition(start, end, ch);
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

