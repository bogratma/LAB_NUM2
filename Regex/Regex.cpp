//
// Created by vlad on 3/25/26.
//

#include "Regex.h"
#include <iostream>
#include "algorithm"
#include "../StateElimination/StateElim.h"
void Regex::comp() {
    if (is_compiled) return;
    compileToMDFA(mainRoot.get(), mainAutomata, "main");
    if (has_lookahead && lookRoot) {
        compileToMDFA(lookRoot.get(), lookaheadAutomata, "lookahead");
    }
    is_compiled = true;
}
void Regex::compileToMDFA(Node* treeRoot, MDFA& target, const std::string& prefix) {
    if (!treeRoot) return;
        /*if (treeRoot->type==INTERSECT) {
            MDFA left;
            MDFA right;
            compileToMDFA(treeRoot->left.get(),left,"L");
            compileToMDFA(treeRoot->right.get(),right,"R");
            target = MDFA::diff(left,right,false);
            target.dumpDOT("res.dot");
            return;
        }*/
        NFA nfa;
        nfa.compile(treeRoot);
        nfa.dumpDot(prefix + "nfa.dot");
        DFA dfa;
        dfa.process(nfa);
        dfa.dumpDot(prefix + "dfa.dot");
        target.minimize(dfa);
        target.dumpDOT(prefix + "mdfa.dot");

}

bool Regex::match(const std::string& s) {
    if (!is_compiled) comp();
    const std::vector<int> points = mainAutomata.getAllFinInd(s);
    if (has_lookahead) {
        for (int i = points.size() - 1; i >= 0; --i) {
            const int pos = points[i];
            std::string tail = s.substr(pos);
            if (lookaheadAutomata.match(tail)) {
                return true;
            }
        }
        return false;
    }
    if (points.empty()) return false;
    return points.back() == s.length();
}

bool Regex::search(const std::string& s) {
    if (!is_compiled) comp();
    const std::vector<int> points = mainAutomata.getAllFinInd(s);
    if (has_lookahead) {
        for (int i = points.size() - 1; i >= 0; --i) {
            const int pos = points[i];
            std::string tail = s.substr(pos);
            if (lookaheadAutomata.search(tail)) {
                return true;
            }
        }
        return false;
    }
    return !points.empty();
}