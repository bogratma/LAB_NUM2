//
// Created by vlad on 3/31/26.
//

#include "StateElim.h"
void StateElim::init(const MDFA& mdfa) {
    table.clear();
    for (auto const& [from, moves] : mdfa.tableTransitionMDFA) {
        for (auto const& [sym, to] : moves) {
            const std::string s(1, sym);
            if (table[from].contains(to)) {
                table[from][to] = '('+table[from][to]+'|'+s+')';
            } else {
                table[from][to] = s;
            }
        }
    }
    const int fictStart = -1;
    int fictEnd = -2;
    table[fictStart][mdfa.startMDFA] = "$";
    for (int finalState : mdfa.finalPi) {
        if (table[finalState].contains(fictEnd)) {
            table[finalState][fictEnd] ="("+table[finalState][fictEnd]+"|$"+")";
        } else {
            table[finalState][fictEnd] = "$";
        }
    }
}
void StateElim::eliminateState(int q) {
    std::string loop;
    std::string s = table[q][q];
    if (s != "$" && !s.empty()) {
        loop = "(" + s + ")*";
    }

    std::vector<int> predecessors;
    for (auto const& [p, moves] : table) {
        if (p != q && moves.contains(q)) predecessors.push_back(p);
    }
    for (int p : predecessors) {
        const std::string to_q = table[p][q];
        for (auto const& [r, from_q_regex] : table[q]) {
            if (r == q) continue;
            const std::string path = table[p][q]+loop+from_q_regex;
            if (table[p].contains(r)) {
                table[p][r] = "(" + table[p][r] + "|" + path + ")";
            } else {
                table[p][r] = path;
            }
        }
    }
    table.erase(q);
    for (auto &moves: table | std::views::values) {
        moves.erase(q);
    }
}
std::string StateElim::getRegex(const MDFA& mdfa) {
    init(mdfa);
    std::vector<int> statesToEliminate;
    for (const auto &state: table | std::views::keys) {
        if (state != -1 && state != -2) {
            statesToEliminate.push_back(state);
        }
    }
    for (const int q : statesToEliminate) {
        eliminateState(q);
    }
    if (table[-1].contains(-2)) {
        return table[-1][-2];
    }

    return "";
}