//
// Created by vlad on 3/14/26.
//

#ifndef LAB_NUM2_NFA_H
#define LAB_NUM2_NFA_H
#include <memory>
#include <unordered_map>
#include <vector>
#include "../tree/Node.h"
#include "State.h"

struct reHash {
    size_t operator()(const std::pair<State*,char>& s) const {
        size_t h = std::hash<State*>{}(s.first);
        return h;
    }
};
class NFA {
public:
    std::vector<std::unique_ptr<State>> states;
    std::unordered_multimap<std::pair<State*,char>,State*,reHash> table;
    State* entry = nullptr;
    State* create(const std::string& name) {
        states.push_back(std::make_unique<State>(name));
        return states.back().get();
    }
    void addTransition(State* f, State* s, char c) {
        table.emplace(std::make_pair(f, c), s);
    }
    void compile(Node* root);
};
#endif //LAB_NUM2_NFA_H