//
// Created by vlad on 3/20/26.
//

#include "MDFA.h"

#include <iostream>
#include <queue>

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

MDFA MDFA::diff(MDFA& A, MDFA &B, bool d) {
    MDFA dif;
    std::set<char> alphabet = A.alphabet;
    alphabet.insert(B.alphabet.begin(), B.alphabet.end());
    std::map<std::pair<int, int>, int> pairs;
    std::queue<std::pair<int, int>> q;
    const std::pair startPair = {A.startMDFA, B.startMDFA};
    pairs[startPair] = 0;
    dif.startMDFA = 0;
    q.push(startPair);
    int nextId = 1;
    while (!q.empty()) {
        std::pair<int, int> currPair = q.front();
        q.pop();
        int newId = pairs[currPair];
        bool aFinal = currPair.first != -1 && A.finalPi.contains(currPair.first);
        bool bFinal = currPair.second != -1 && B.finalPi.contains(currPair.second);
        bool pick = false;
        if (d) pick = aFinal && !bFinal;
        else pick = bFinal && aFinal;
        if (pick) {
            dif.finalPi.insert(newId);
        }
        for (char c : alphabet) {
            int nextA = -1;
            if (currPair.first != -1 && A.tableTransitionMDFA.contains(currPair.first) && A.tableTransitionMDFA.at(currPair.first).contains(c)) {
                nextA = A.tableTransitionMDFA.at(currPair.first).at(c);
            }
            int nextB = -1;
            if (currPair.second != -1 && B.tableTransitionMDFA.contains(currPair.second) && B.tableTransitionMDFA.at(currPair.second).contains(c)) {
                nextB = B.tableTransitionMDFA.at(currPair.second).at(c);
            }
            std::pair nextPair = {nextA, nextB};
            if (!pairs.contains(nextPair)) {
                pairs[nextPair] = nextId++;
                q.push(nextPair);
            }
            dif.tableTransitionMDFA[newId][c] = pairs[nextPair];
        }
    }
    dif.alphabet = alphabet;
    dif.selfMin();
    return dif;
}
void MDFA::selfMin() {
    DFA temp;
    temp.startDFA =this->startMDFA;
    temp.finalDFA =this->finalPi;
    temp.alphabet = this->alphabet;
    temp.transitionTable = this->tableTransitionMDFA;
    this->minimize(temp);
}
void MDFA::initStackEl(std::map<std::pair<int, int>, std::string>& R, int S, int F) {
    for (auto const& [from, transitions] : tableTransitionMDFA) {
        for (auto const& [sym, to] : transitions) {
            std::string s(1, sym);
            if (R.contains({from, to})) R[{from, to}] = R[{from, to}] + "|" + s;
            else R[{from, to}] = s;
        }
    }
    R[{S, startMDFA}] = "";
    for (int f : finalPi) {
        if (R.contains({f, F})) R[{f, F}] = "(" + R[{f, F}] + "|)";
        else R[{f, F}] = "";
    }
}
std::string wrap(const std::string& s) {
    if (s.empty()) return "";
    if (s.length() == 1) return s;
    if (s.front() == '(' && s.back() == ')') {
        int bal = 0;
        bool split = false;
        for (size_t i = 0; i < s.size() - 1; ++i) {
            if (s[i] == '(') bal++;
            if (s[i] == ')') bal--;
            if (bal == 0) { split = true; break; }
        }
        if (!split) return s;
    }
    return "(" + s + ")";
}
std::pair<std::vector<int>, std::vector<int>> MDFA::getNeighbors(const std::map<std::pair<int, int>, std::string>& R, int k) {
    std::set<int> pre, succ;
    for (auto const& [edge, expr] : R) {
        if (edge.second == k && edge.first != k) pre.insert(edge.first);
        if (edge.first == k && edge.second != k) succ.insert(edge.second);
    }
    return { {pre.begin(), pre.end()}, {succ.begin(), succ.end()} };
}
std::string MDFA::toRegex() {
    std::map<std::pair<int, int>, std::string> R;
    int S = -1, F = -2;

    initStackEl(R, S, F);

    // Удаляем все промежуточные состояния
    for (int k = 0; k < Pi.size(); ++k) {
        auto [precursors, successors] = getNeighbors(R, k);

        for (int i : precursors) {
            for (int j : successors) {
                // Извлекаем текущие выражения
                std::string r_ik = R[{i, k}];
                std::string r_kj = R[{k, j}];
                std::string r_kk = R.count({k, k}) ? R[{k, k}] : "";
                std::string r_ij = R.count({i, j}) ? R[{i, j}] : "";

                // Формируем путь через k: r_ik (r_kk)* r_kj
                std::string path = "";

                // 1. Идем из i в k
                if (r_ik.find('|') != std::string::npos) path += "(" + r_ik + ")";
                else path += r_ik;

                // 2. Крутимся в k (петля)
                if (!r_kk.empty()) {
                    if (r_kk.length() > 1) path += "(" + r_kk + ")*";
                    else path += r_kk + "*";
                }

                // 3. Идем из k в j
                if (r_kj.find('|') != std::string::npos) path += "(" + r_kj + ")";
                else path += r_kj;

                // 4. Объединяем с прямым путем i -> j
                if (r_ij.empty()) {
                    R[{i, j}] = path;
                } else {
                    // Чтобы не плодить скобки, проверяем r_ij
                    R[{i, j}] = r_ij + "|" + path;
                }
            }
        }

        // КРИТИЧЕСКИ ВАЖНО: Стираем k из графа, чтобы он не всплыл на итерации k+1
        std::vector<std::pair<int, int>> keysToRemove;
        for (auto const& [edge, expr] : R) {
            if (edge.first == k || edge.second == k) {
                keysToRemove.push_back(edge);
            }
        }
        for (auto const& key : keysToRemove) R.erase(key);
    }

    // Финальный результат — путь от супер-старта к супер-финишу
    std::string finalRes = R[{S, F}];

    // Если результат пустой, значит автомат ничего не принимает
    if (finalRes.empty()) return "∅";

    return finalRes;
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