//
// Created by vlad on 3/14/26.
//

#ifndef LAB_NUM2_NFA_H
#define LAB_NUM2_NFA_H
#include <memory>
#include <set>
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
    std::set<char> alphabet;
    std::unordered_multimap<std::pair<State*,char>,State*,reHash> table;
    State* entry = nullptr;

    State* create() {
        states.push_back(std::make_unique<State>());
        return states.back().get();
    }
    void addTransition(State* f, State* s, char c) {
        alphabet.emplace(c);
        table.emplace(std::make_pair(f, c), s);
    }
    void compile(Node* root);
    void dumpDot(const std::string& filename) {
        std::ofstream out(filename);
        out << "digraph NFA {\n";
        out << "    rankdir=LR;\n";
        out << "    nodesep=0.5;\n";
        out << "    ranksep=1.0;\n";
        out << "    splines=curved;\n";
        out << "    edge [arrowsize=0.8];\n";
        out << "    node [fontname=\"Arial\", fontsize=12];\n";
        for (const auto& s : states) {
            std::string shape = s->isAcceptable ? "doublecircle" : "circle";
            out << "    \"" << s->id << "\" [shape=" << shape << ", width=0.6, fixedsize=true];\n";
        }
        if (!states.empty()) {
            out << "    node [shape=none, width=0]; start_node [label=\"\"];\n";
            out << "    start_node -> \"" << entry->id << "\";\n";
        }

        for (auto const& [key, targetStruct] : table) {
            State* from = key.first;
            char symbol = key.second;
            State* target = targetStruct;
            std::string label = (symbol == '$') ? "ε" : std::string(1, symbol);
            out << "    \"" << from->id << "\" -> \"" << target->id
                << "\" [label=\"" << label << "\"];\n";
        }
        out << "}\n";
        out.close();
    }

};

#endif //LAB_NUM2_NFA_H