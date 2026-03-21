//
// Created by vlad on 3/20/26.
//

#include "MDFA.h"

#include <iostream>

std::set<int> getFinal(const DFA& dfa) {
    return dfa.finalDFA;
}
std::set<int> notFinal(const DFA& dfa) {
    std::set<int> noDFA;
    for (const auto& c: dfa.transitionTable) {
        if (!dfa.finalDFA.contains(c.first)) noDFA.insert(c.first);
    }
    return noDFA;
}
int findGroupInd(int id, std::vector<std::set<int>>& groups) {
    int idx = -1;
    for (int i = 0; i < groups.size(); i++) {
        if (groups[i].contains(id)) idx = i;
    }
    return idx;
}
bool MDFA::checkGroup(DFA& dfa , const std::set<int>& group) {
   if (group.size()<=1) return true;
    const int first = *group.begin();
    for (char c :dfa.alphabet) {
        int target = -1;
        if (dfa.transitionTable[first].contains(c)) {
            target = dfa.transitionTable[first][c];
        }
        const int groups  = findGroupInd(target, Pi);
        for (int s : group) {
            int targets = -1;
            if (dfa.transitionTable[first].contains(c)) {
                targets = dfa.transitionTable[s][c];
            }
            int cur = findGroupInd(targets,Pi);
            if (cur!=groups) return false;
        }
    }
    return true;
}
std::vector<std::set<int>> MDFA::split(const std::set<int>& group, DFA& dfa) {
    std::map<std::vector<int>,std::set<int>> groups;
    for (auto state : group) {
        std::vector<int> b;
        for (char sym : dfa.alphabet) {
            int target = -1;
            if (dfa.transitionTable[state].contains(sym)) {
                target = dfa.transitionTable[state][sym];
            }
            b.push_back(findGroupInd(target,Pi));
        }
        groups[b].insert(state);
    }
    std::vector<std::set<int>> result;
    for (const auto& c: groups) {
        result.push_back(c.second);
    }
    return result;
}
void MDFA::buildMFDA(DFA& dfa) {
    this->states = Pi.size();
    this->tableTransitionMDFA.clear();
    this->finalPi.clear();
    this->startMDFA = findGroupInd(dfa.startDFA, Pi);
    for (int i =0 ; i<Pi.size();++i) {
        int rep = *Pi[i].begin();
         if (dfa.finalDFA.contains(rep)) {
             this->finalPi.insert(i);
         }
        for (char sym : dfa.alphabet) {
            if (dfa.transitionTable[rep].contains(sym)) {
                int oldTarget = dfa.transitionTable[rep][sym];
                int newTarget = findGroupInd(oldTarget,Pi);
                this->tableTransitionMDFA[i][sym] = newTarget;
            }
        }
    }
}
void MDFA::minimize(DFA& dfa) {
std::set<int> notfinal = notFinal(dfa);
std::set<int> final = getFinal(dfa);
    Pi.push_back(final);
    Pi.push_back(notfinal);
    while (true) {
        std::vector<std::set<int>> nexts;
        int old = Pi.size();
        for ( const auto& f: Pi) {
            if (checkGroup(dfa,f)) {
                nexts.push_back(f);
            }
            else {
                std::vector<std::set<int>> splitted = split(f,dfa);
                nexts.insert(nexts.end(),splitted.begin(),splitted.end());
            }
        }
        Pi = nexts;
        if (Pi.size()==old) break;
    }
    buildMFDA(dfa);
}
void MDFA::dumpDOT(std::string filename) {
    std::ofstream out(filename);
    out << "digraph MDFA {" << std::endl;
    out << "rankdir=LR;" << std::endl;
    out << "node [shape = circle];" << std::endl;
    out << "node [shape = none, label=\"\"]; start_node;" << std::endl;
    out << "start_node -> " << this->startMDFA << ";" << std::endl;
    for (int i = 0; i < this->states; ++i) {
        if (this->finalPi.contains(i)) {
            out << "    " << i << " [shape = doublecircle, label=\"" << i << "\"];" << std::endl;
        } else {
            out << "    " << i << " [shape = circle, label=\"" << i << "\"];" << std::endl;
        }
    }
    for (auto const& [fromNode, transitions] : this->tableTransitionMDFA) {
        for (auto const& [symbol, toNode] : transitions) {
            out << "    " << fromNode << " -> " << toNode
                << " [label = \"" << symbol << "\"];" << std::endl;
        }
    }
    out << "}" << std::endl;
    out.close();
}