//
// Created by vlad on 3/14/26.
//

#include "NFA.h"

#include <stack>

#include "../tree/Node.h"
#include "Chunk.h"
void NFA::compile(Node* root) {
std::vector<char> names;
    std::stack<Chunk> base;
postOrder(root,names);
    int i=0;
    for (char c : names) {
        if (c =='a' || c=='b') {
            State* start = create(std::to_string(i));
            ++i;
            State* end = create(std::to_string(i));
            ++i;
            addTransition(start,end,c);
            end->isAcceptable=true;
            base.emplace(start,end);
        }
        else if (c=='|') {
            State* start = create(std::to_string(i));
            ++i;
            State* end = create(std::to_string(i));
            ++i;
            end->isAcceptable=true;
            Chunk top = base.top();
            base.pop();
            Chunk bottom = base.top();
            base.pop();
            top.end->isAcceptable = false;
            bottom.end->isAcceptable = false;
            addTransition(start,top.start,'$');
            addTransition(start,bottom.start,'$');
            addTransition(top.end,end,'$');
            addTransition(bottom.end,end,'$');
            base.emplace(start,end);
        }
        else if (c=='*') {
            State* start = create(std::to_string(i));
            ++i;
            State* end = create(std::to_string(i));
            ++i;
            end->isAcceptable=true;
            addTransition(start,end,'$');
            Chunk top = base.top();
            top.end->isAcceptable = false;
            base.pop();
            addTransition(start, top.start,'$');
            addTransition(top.end,top.start,'$');
            addTransition(top.end,end,'$');
            base.emplace(start,end);
        }
        else if (c=='.') {
            Chunk second = base.top();
            base.pop();
            Chunk first = base.top();
            base.pop();
            first.end->isAcceptable = false;
            addTransition(first.end,second.start,'$');
            base.emplace(first);
        }
    }
    entry = base.top().start;
    base.pop();
}
