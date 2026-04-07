//
// Created by vlad on 3/20/26.
//

#include "MDFA.h"

#include <iostream>
#include <queue>
#include <ranges>

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
int findGroupInd(int id, const std::vector<std::set<int>>& groups) {
    int idx = -1;
    for (int i = 0; i < groups.size(); i++) {
        if (groups[i].contains(id)) //idx = i;
            return i;
    }
    return idx;
}
bool MDFA::checkGroup(DFA& dfa , const std::set<int>& group) const {
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
            if (dfa.transitionTable[s].contains(c)) {
                targets = dfa.transitionTable[s][c];
            }
            int cur = findGroupInd(targets,Pi);
            if (cur!=groups) return false;
        }
    }
    return true;
}
std::vector<std::set<int>> MDFA::split(const std::set<int>& group, DFA& dfa) const {
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
                this->alphabet.insert(sym);
            }
        }
    }
}
void MDFA::minimize(DFA& dfa) {
    Pi.clear();
std::set<int> notfinal = notFinal(dfa);
std::set<int> final = getFinal(dfa);
   if (!final.empty()) Pi.push_back(final);
   if (!notfinal.empty()) Pi.push_back(notfinal);
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

bool MDFA::match(const std::string& str) {
    int start = this->startMDFA;
    for (char c : str) {
        if (!tableTransitionMDFA[start].contains(c)) return false;
        start = tableTransitionMDFA[start][c];
    }
    return finalPi.contains(start);
}
bool MDFA::search(const std::string& str) {
    for (int i=0; i <str.length(); i++) {
        int start = this->startMDFA;
        for (int j=i; j <str.length(); j++) {
            char c = str[j];
            if (!tableTransitionMDFA[start].contains(c)) break;
            start = tableTransitionMDFA[start][c];
            if (finalPi.contains(start)) return true;
        }
        if (finalPi.contains(this->startMDFA)) return true;
    }
    return false;
}


void MDFA::selfMin() {
    DFA temp;
    temp.startDFA =this->startMDFA;
    temp.finalDFA =this->finalPi;
    temp.alphabet = this->alphabet;
    temp.transitionTable = this->tableTransitionMDFA;
    this->minimize(temp);
}

std::vector<int> MDFA::getAllFinInd(const std::string& s) {
    std::vector<int> indices;
    int currentState = startMDFA;
    if (finalPi.contains(currentState)) {
        indices.push_back(0);
    }
    for (int i = 0; i < s.length(); ++i) {
        char c = s[i];
        if (!tableTransitionMDFA[currentState].contains(c)) {
            break;
        }
        currentState = tableTransitionMDFA[currentState][c];
        if (finalPi.contains(currentState)) {
            indices.push_back(i + 1);
        }
    }
    return indices;
}

Product MDFA::getProduct(const MDFA& A,const MDFA& B)  {
    Product product;
    product.alphabet = A.alphabet;
    product.alphabet.insert(B.alphabet.begin(),B.alphabet.end());
    std::queue<std::pair<int,int>> q;
    const std::pair startPair = {A.startMDFA,B.startMDFA};
    product.pairs[startPair] = 0;
    q.push(startPair);
    int nextId = 1;
    while (!q.empty()) {
        auto currPair = q.front();
        q.pop();
        int curId = product.pairs[currPair];
        for (char c: product.alphabet ) {
            int nextA = -1;
            int nextB = -1;
            if (currPair.first!=-1 && A.tableTransitionMDFA.contains(currPair.first)
                && A.tableTransitionMDFA.at(currPair.first).contains(c)) {
                nextA = A.tableTransitionMDFA.at(currPair.first).at(c);
            }
            if (currPair.second!=-1 && B.tableTransitionMDFA.contains(currPair.first)
                && B.tableTransitionMDFA.at(currPair.second).contains(c)) {
                nextB = B.tableTransitionMDFA.at(currPair.second).at(c);
            }
            std::pair nextPair = {nextA,nextB};
            if (!product.pairs.contains(nextPair)) {
                product.pairs[nextPair] = nextId++;
                q.push(nextPair);
            }
            product.transitions[curId][c] =product.pairs[nextPair];
        }
    }
    return  product;
}

MDFA MDFA::diff(const MDFA& A,  const MDFA& B, const bool d)  {
    Product product = getProduct(A, B);
    MDFA res;
    res.alphabet =product.alphabet;
    res.tableTransitionMDFA = product.transitions;
    res.startMDFA = 0;
    for (auto const& [pair,id] : product.pairs) {
            const bool aFinal = pair.first != -1 && A.finalPi.contains(pair.first) ;
            const bool bFinal = pair.second != -1 && B.finalPi.contains(pair.second);
        if (d) {
            if (aFinal && !bFinal) {
                res.finalPi.insert(id);
            }
        }else {
            if (aFinal && bFinal) {
                res.finalPi.insert(id);
            }
        }
    }
    res.selfMin();
    return res;
}
bool MDFA::equal(const MDFA& A,const MDFA& B) {
    const MDFA diff1 = diff(A,B,true);
    const MDFA diff2 = diff(B,A,true);
    return diff1.isEmpty() && diff2.isEmpty();
}

bool MDFA::isEmpty() const {
    if (finalPi.empty()) {
        return true;
    }
    std::queue<int> q;
    std::set<int> visited;
    q.push(startMDFA);
    visited.insert(startMDFA);
    while (!q.empty()) {
        int curr = q.front();
        q.pop();
        if (finalPi.contains(curr)) {
            return false;
        }
        if (tableTransitionMDFA.contains(curr)) {
            for (const auto &nextState: tableTransitionMDFA.at(curr) | std::views::values) {
                if (nextState != -1 && !visited.contains(nextState)) {
                    visited.insert(nextState);
                    q.push(nextState);
                }
            }
        }
    }
    return true;
}
void MDFA::dumpDOT(const std::string& filename) {
    std::ofstream out(filename);
    out << "digraph MDFA {" << std::endl;
    out << "rankdir=LR;" << std::endl;
    out << "node [shape = circle];" << std::endl;
    out << "node [shape = none, label=\"\"]; start_node;" << std::endl;
    out << "start_node -> " << this->startMDFA << ";" << std::endl;
    for (int i = 0; i < Pi.size(); ++i) {
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