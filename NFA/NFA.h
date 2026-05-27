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
    void addEps(State* f, State* s, std::vector<Tag> tags, int p = 0) {
        f->eps.push_back({s,p,std::move(tags)});
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
            out << "    \"" << s->id << "\" [shape=" << shape
                << ", width=0.6, fixedsize=true];\n";
        }
        out << "    node [shape=none, width=0]; start_node [label=\"\"];\n";
        out << "    start_node -> \"" << entry->id << "\";\n";
        for (auto const& [key, target] : table) {
            State* from  = key.first;
            char  symbol = key.second;
            out << "    \"" << from->id << "\" -> \"" << target->id
                << "\" [label=\"" << std::string(1, symbol) << "\"];\n";
        }
        for (const auto& s : states) {
            for (const auto& t : s->eps) {
                std::string label = "&epsilon;";
                if (!t.tags.empty()) {
                    label += " [";
                    for (int i = 0; i < (int)t.tags.size(); i++) {
                        int reg = t.tags[i].reg;
                        int g   = reg / 2;
                        label  += "r" + std::to_string(reg)
                               +  "(g" + std::to_string(g)
                               +  (reg % 2 == 0 ? "_start" : "_end") + ")";
                        if (i + 1 < (int)t.tags.size()) label += ",";
                    }
                    label += "]";
                }
                out << "    \"" << s->id << "\" -> \"" << t.to->id
                    << "\" [label=\"" << label << "\", style=dashed];\n";
            }
            for (const auto& bt : s->backs) {
                out << "    \"" << s->id << "\" -> \"" << bt.to->id
                    << "\" [label=\"\\\\" << (bt.group + 1)
                    << "\", style=dotted, color=red];\n";
            }
        }
        out << "}\n";
    }
};

#endif //LAB_NUM2_NFA_H